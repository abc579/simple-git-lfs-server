#include "httplib_lfs_server.h"

#include "base64.h"
#include "config.h"
#include "httplib.h"
#include "istorage.h"
#include "json11.hpp"
#include "util.h"

using httplib::Request;
using httplib::Response;
using httplib::SSLServer;
using storage::istorage;

using namespace lfs;
using namespace server;
using namespace util;

httplib_lfs_server::httplib_lfs_server(config& cfg, istorage& storage) {
  cfg_ = &cfg;
  storage_ = &storage;
  server_ = std::make_unique<SSLServer>(cfg.cert().c_str(), cfg.key().c_str());
}

void httplib_lfs_server::listen() {
  set_batch_handler();
  set_download_handler();
  set_upload_handler();
  set_verify_handler();
  set_exception_handler();
  server_->listen(cfg_->host(), cfg_->port());
}

void httplib_lfs_server::set_batch_handler() {
  server_->Post("/objects/batch", [&](const auto& request, auto& response) {
    if (process_auth(request, response)) {
      std::string err;
      auto req = parse_json(request.body, err);
      if (!err.empty()) {
        throw json_parse_error{"batch_request_handler(): Bad JSON format."};
      }
      const auto auth = request.headers.find("Authorization")->second;
      process_batch_request(req, auth, response);
      response.status = static_cast<int>(http_response_codes::ok);
    }
  });
}

void httplib_lfs_server::set_download_handler() {
  server_->Get(R"(/objects/[a-zA-Z0-9]*)", [&](const auto& request,
                                               auto& response) {
    if (process_auth(request, response)) {
      const auto oid = get_path_from_url(request.path);
      try {
        const auto binary_file = storage_->read_file(oid);
        response.set_header("Accept", ACCEPT_LFS);
        response.set_content(binary_file, "application/octet-stream");
        response.status = static_cast<int>(http_response_codes::ok);
      } catch (const storage::storage_exception& se) {
        // @TEMP: I do not think the Git LFS client cares about this internal
        // error...
        response.status = static_cast<int>(http_response_codes::internal_error);
        std::cerr << se.what() << std::endl;
      }
    }
  });
}

void httplib_lfs_server::set_upload_handler() {
  server_->Put(R"(/[a-zA-Z0-9]*)", [&](const auto& request, auto& response) {
    if (process_auth(request, response)) {
      const auto oid = get_path_from_url(request.path);
      try {
        storage_->write_file(oid, request.body);
        response.status = static_cast<int>(http_response_codes::ok);
      } catch (const storage::storage_exception& se) {
        response.status = static_cast<int>(http_response_codes::internal_error);
        std::cerr << se.what() << std::endl;
      };
    }
  });
}

void httplib_lfs_server::set_verify_handler() {
  server_->Post("/verify", [&](const auto& request, auto& response) {
    std::string msg;
    const auto object = parse_json(request.body, msg);
    if (!msg.empty()) {
      throw json_parse_error{"verify_handler(): Error parsing JSON."};
    }
    const auto oid = object["oid"].string_value();
    if (!storage_->file_exists(oid)) {
      response.status = static_cast<int>(object_error_codes::not_found);
    } else {
      response.status = static_cast<int>(http_response_codes::ok);
    }
  });
}

// @NOTE: the API does not specify clearly what to do if something like
// parsing a JSON goes bad. I have tried the reference Go server and
// they just return the following: { "objects" : "null }. We will do the
// same thing for now; notice that all exceptions that occur while the
// server is running will get here.
void httplib_lfs_server::set_exception_handler() {
  server_->set_exception_handler([&](const auto&, auto& response, auto) {
    response.status = static_cast<int>(http_response_codes::ok);
    response.set_content(R"({"objects":null})", CONTENT_TYPE_LFS);
  });
}

void httplib_lfs_server::stop() {
  server_->Get("/stop", [&](const auto&, auto&) { server_->stop(); });
}

bool httplib_lfs_server::authenticate(const Request& request) {
  const auto auth = request.get_header_value("Authorization");
  const std::string prefix{"Basic "};
  if (auth.empty()) {
    return false;
  }
  if (auth.find(prefix) == std::string::npos) {
    return false;
  }
  const auto credentials = get_credentials_from_base64(auth, prefix);
  if (credentials.first.empty()) {
    return false;
  }
  return credentials.first == cfg_->user() &&
         credentials.second == cfg_->passwd();
}

bool httplib_lfs_server::process_auth(const Request& request,
                                      Response& response) {
  if (!authenticate(request)) {
    response.status = static_cast<int>(http_response_codes::auth_required);
    response.set_content(AUTH_ERROR, CONTENT_TYPE_LFS);
    return false;
  }
  return true;
}

std::pair<std::string, std::string>
httplib_lfs_server::get_credentials_from_base64(const std::string& auth,
                                                const std::string& prefix) {
  const auto encoded = auth.substr(prefix.length());
  if (encoded.empty()) {
    return {"", ""};
  }
  const auto decoded = base64_decode(encoded);
  const auto colon_pos = decoded.find(':');
  return {decoded.substr(0, colon_pos), decoded.substr(colon_pos + 1)};
}

void httplib_lfs_server::process_batch_request(const json_t& request,
                                               const std::string& authorization,
                                               Response& response) {
  const auto objects = request["objects"].array_items();
  // @NOTE: the API is silent about this, but since there is no reasonable
  // thing to do, we will throw an exception.
  if (objects.empty()) {
    throw json_parse_error{"`objects` key is mandatory."};
  }
  auto temp_op = request["operation"].string_value();
  // @NOTE: again, the API does not tell what to do if the request does
  // not specify the operation or the operation is not download or
  // upload.  In our case, we are going to assume that the client wants
  // to download.
  if (temp_op.empty() || (temp_op != "download" && temp_op != "upload")) {
    temp_op = "download";
  }
  const auto operation = temp_op;
  batch_response br{create_batch_response(operation, objects, authorization)};
  response.set_header("Accept", ACCEPT_LFS);
  response.set_content(encode_batch_response(br, operation), CONTENT_TYPE_LFS);
  response.status = static_cast<int>(http_response_codes::ok);
}

void httplib_lfs_server::create_download_response(
    const json_array_t& objects, const std::string& authorization,
    batch_response& br) {
  static const error err{"Object does not exist.",
                         static_cast<int>(object_error_codes::not_found)};
  for (const auto& o : objects) {
    const auto oid = o["oid"].string_value();
    const auto size = o["size"].int_value();
    object obj{oid, size};
    if (oid.empty() || !storage_->file_exists(oid)) {
      br.objects.push_back({{}, {}, err, obj});
      continue;
    }
    // If the size does not come in the request or they provided zero,
    // obtain it.
    if (size == 0) {
      obj.size = storage_->get_file_size(oid);
    }
    operation actions = {
        {authorization},
        get_url(cfg_->scheme(), cfg_->host(), std::to_string(cfg_->port()),
                cfg_->get_download_object_path() + obj.oid),
    };
    br.objects.push_back({actions, {}, {}, obj});
  }
}

void httplib_lfs_server::create_upload_response(
    const json_array_t& objects, const std::string& authorization,
    batch_response& br) {
  static const error empty_error{};
  for (const auto& o : objects) {
    object obj{o["oid"].string_value(), o["size"].int_value()};
    operation actions = {
        {authorization},
        get_url(cfg_->scheme(), cfg_->host(), std::to_string(cfg_->port()),
                cfg_->get_upload_object_path() + obj.oid),
    };
    verify verify = {
        {authorization},
        get_url(cfg_->scheme(), cfg_->host(), std::to_string(cfg_->port()),
                cfg_->get_verify_object_path()),
    };
    br.objects.push_back({actions, verify, empty_error, obj});
  }
}

// @TODO: naming.
batch_response httplib_lfs_server::create_batch_response(
    const std::string& op, const json_array_t& objects,
    const std::string& authorization) {
  batch_response br{};
  if (op == "download") {
    create_download_response(objects, authorization, br);
  } else if (op == "upload") {
    create_upload_response(objects, authorization, br);
  }
  return br;
}

void httplib_lfs_server::encode_upload_batch_response(
    const lfs::batch_response& br, json_array_t& object_array) {
  for (const auto& o : br.objects) {
    json_object_t jtemp;
    jtemp = {
        {"oid", o.obj_.oid},
        {"size", o.obj_.size},
        {"authenticated", o.authenticated},
        {"actions",
         json_object_t{
             {"upload",
              json_object_t{
                  {"href", o.actions_.href},
                  {"header", json_object_t{{"Authorization",
                                            o.actions_.header_.authorization}}},
                  {"expires_in", o.actions_.expires_in}}},
             {"verify",
              json_object_t{
                  {"href", o.verify_.href},
                  {"header", json_object_t{{"Authorization",
                                            o.verify_.header_.authorization}}},
                  {"expires_in", o.verify_.expires_in}}}}}};
    object_array.push_back(jtemp);
  }
}

void httplib_lfs_server::encode_download_batch_response(
    const batch_response& br, json_array_t& object_array) {
  for (const auto& o : br.objects) {
    json_object_t jtemp;
    if (o.err_.message.empty()) {
      jtemp = {{"oid", o.obj_.oid},
               {"size", o.obj_.size},
               {"authenticated", o.authenticated},
               {"actions",
                json_object_t{
                    {"download",
                     json_object_t{
                         {"href", o.actions_.href},
                         {"header",
                          json_object_t{{"Authorization",
                                         o.actions_.header_.authorization}}},
                         {"expires_in", o.actions_.expires_in}}}}}};
    } else {
      jtemp = {{"oid", o.obj_.oid},
               {"size", o.obj_.size},
               {"error", json_object_t{{"code", o.err_.code},
                                       {"message", o.err_.message}}}};
    }
    object_array.push_back(jtemp);
  }
}

std::string httplib_lfs_server::encode_batch_response(
    const batch_response& br, const std::string& operation) {
  json_object_t j = {{"transfer", br.transfer}, {"hash_algo", br.hash_algo}};
  json_array_t object_array;
  if (operation == "upload") {
    encode_upload_batch_response(br, object_array);
  } else if (operation == "download") {
    encode_download_batch_response(br, object_array);
  }
  j["objects"] = object_array;
  return json_t(j).dump();
}
