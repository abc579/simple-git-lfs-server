#include <cstddef>
#include <cstdint>
#include <ios>
#include <iterator>
#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <exception>
#include <algorithm>
#include <cstdint>

#include "json.h"
#include "json11.hpp"
#include "logger.h"
#include "simple_git_lfs.h"
#include "server_config.h"
#include "base64.h"
#include "util.h"

void lfs::batch_request_handler(const request_t& request, response_t& response, const server_config::data& cfg,
                                lfs::log& logger)
{
  auto req = json(request.body);
  const auto auth = request.headers.find("Authorization")->second;

  // Notice that `process_batch_request` throws an exception and we do not catch it here;
  // this is intented: when we throw an exception the lambda defined in `main` by calling
  // `set_exception_handler` will actually do the job.
  process_batch_request(req, response, cfg, auth, logger);

  response.status = static_cast<int>(http_response_codes::ok);
}

void lfs::process_batch_request(const json& request, response_t& response,
                                const server_config::data& cfg,
                                const std::string& authorization,
                                lfs::log& logger)
{
  const auto objects = request.get_array_items("objects");

  // @NOTE(lev): API is silent about this, but since there is no
  // reasonable thing to do, we will throw an exception.
  if (objects.empty()) {
    throw json_parse_error {"`objects` key is mandatory."};
  }

  auto temp_op = request.get_string_value("operation");

  // @NOTE(lev): again, the API does not tell what to do if the request
  // does not specify the operation or the operation is not download or upload.
  // In our case, we are going to assume that the client wants to download.
  if (temp_op.empty() || (temp_op != "download" && temp_op != "upload")) {
    temp_op = "download";
  }

  const auto operation = temp_op;

  batch_response br {create_batch_response(operation, objects, cfg, authorization, logger)};

  response.set_header("Accept", ACCEPT_LFS);

  response.set_content(encode_batch_response(br, operation), CONTENT_TYPE_LFS);
  response.status = static_cast<int>(http_response_codes::ok);
}

lfs::batch_response lfs::create_batch_response(const std::string& operation, const json_array_t& objects,
                                               const server_config::data& cfg, const std::string& authorization,
                                               lfs::log& logger)
{
  static const error_object error {"Object does not exist.", static_cast<int>(object_error_codes::not_found)};

  batch_response br;

  // @TODO(lev): create function to get href.
  if (operation == "download") {
    for (const auto& o : objects) {
	    const auto oid = o["oid"].string_value();
	    const auto size = o["size"].int_value();

	    batch_object bo {oid, size};

	    // Check if the oid is not empty and then if it exists in the file system.
	    if (oid.empty() || !utils::can_open(get_filesystem_path(cfg.file_directory, oid))) {
        br.objects.push_back({bo, {}, {}, error});
        continue;
	    }

	    // If the size does not come in the request or they specified 0, obtain it.
	    if (size == 0) {
        bo.size = utils::get_file_size(get_filesystem_path(cfg.file_directory, oid), logger);
	    }

	    operation_object actions = {
        {authorization}, // Header.
        get_href(cfg.scheme, cfg.host + ':' + std::to_string(cfg.port) + cfg.download_object_path + bo.oid), // Href of operation.
	    };

	    br.objects.push_back({bo, actions, {}, {}});
    }
  } else if (operation == "upload") {
    for (const auto& o : objects) {
	    batch_object bo {o["oid"].string_value(),
                       o["size"].int_value()};

	    operation_object actions = {
        {authorization}, // Header.
        get_href(cfg.scheme, cfg.host + ':' + std::to_string(cfg.port) + cfg.upload_object_path + bo.oid), // Href of operation.
	    };

	    verify_object verify = {
        {authorization},
        get_href(cfg.scheme, cfg.host + ':' + std::to_string(cfg.port) + cfg.verify_object_path), // Href of verify.
	    };

	    br.objects.push_back({bo, actions, verify, {}});
    }
  }

  return br;
}

std::string lfs::encode_batch_response(const batch_response& br, const std::string& operation)
{
  json_object_t j = {
    { "transfer", br.transfer },
    { "hash_algo", HASH_ALGO }
  };

  json_array_t object_array;

  for (const auto& o : br.objects) {
    json_object_t jtemp;

    if (o.error.message.empty()) {
	    // @FIXME(lev): come with something better. The thing is that we have
	    // to provide a "verify" field for uploads, not in downloads. The
	    // rest is the same.
	    if (operation == "upload") {
        jtemp = {
          { "oid", o.object.oid },
          { "size", o.object.size},
          { "authenticated", o.authenticated },
          {   "actions", json_object_t {
              {   operation, json_object_t {
                  { "href", o.actions.href },
                  { "header", json_object_t {
                      {"Authorization", o.actions.header.authorization}
                    }
                  },
                  { "expires_in", o.actions.expires_in}
                }
              },
              { "verify", json_object_t {
                  { "href", o.verify.href},
                  { "header", json_object_t {
                      { "Authorization", o.verify.header.authorization}
                    }
                  },
                  { "expires_in", o.verify.expires_in}
                }
              }
            }
          }
        };
	    } else {
        jtemp = {
          { "oid", o.object.oid },
          { "size", o.object.size},
          { "authenticated", o.authenticated },
          {   "actions", json_object_t {
              {   operation, json_object_t {
                  { "href", o.actions.href },
                  { "header", json_object_t {
                      {"Authorization", o.actions.header.authorization}
                    }
                  },
                  { "expires_in", o.actions.expires_in}
                }
              }
            }
          }
        };
	    }
    } else {
      jtemp = {
        { "oid", o.object.oid},
        { "size", o.object.size},
        {   "error", json_object_t {
            { "code", o.error.code},
            { "message", o.error.message}
          }
        }
      };
    }

    object_array.push_back(jtemp);
  }

  j["objects"] = object_array;

  return json_t(j).dump();
}

// In this case, the OID comes in the URL, so we have to extract that and
// check if the file exists in the directory. If it does, store the contents
// in a string, put that into the response and return it.
void lfs::download_handler(const request_t& request, response_t& response, const server_config::data& cfg)
{
  // @FIXME: when the file is too large, git lfs client does it in chunks using range,
  // so we have to do adapt to that situation.
  const std::string oid = get_oid_from_url(request.path);

  std::ifstream file (get_filesystem_path(cfg.file_directory, oid), std::ios::in | std::ios::binary);
  std::noskipws(file);
  std::vector<unsigned char> binary_file (std::istream_iterator<char>(file), std::istream_iterator<char>{});
  file.close();

  response.set_header("Accept", ACCEPT_LFS);
  response.set_content(std::string(binary_file.begin(), binary_file.end()), "application/octet-stream");
  response.status = (int) http_response_codes::ok;
}

// Git format.
std::string lfs::get_filesystem_path(const std::string& file_directory, const std::string& oid)
{
  const auto oid_data = split_oid(oid);
  return file_directory + '/' + oid_data.parent_dir + '/' + oid_data.child_dir + '/' + oid_data.oid;
}

void lfs::upload_handler(const request_t& request, response_t& response, const server_config::data& cfg,
                         lfs::log& logger)
{
  save_file_in_directory(get_oid_from_url(request.target), request.body, cfg, logger);
  response.status = static_cast<int>(http_response_codes::ok);
}

void lfs::verify_handler(const request_t& request, response_t& response, const server_config::data& cfg)
{
  const auto object = json(request.body);
  const auto oid = object.get_string_value("oid");

  if (!utils::can_open(get_filesystem_path(cfg.file_directory, oid))) {
    response.status = static_cast<int>(object_error_codes::not_found);
  } else {
    response.status = static_cast<int>(http_response_codes::ok);
  }
}

std::string lfs::get_oid_from_url(const std::string& url)
{
  const auto pos = url.find_last_of('/');

  return url.substr(pos + 1);
}

lfs::oid_directory lfs::split_oid(const std::string& oid)
{
  return {
    oid.substr(0, 2),
    oid.substr(2, 2),
    oid.substr(4)
  };
}

void lfs::save_file_in_directory(const std::string& oid, const std::string& raw, const server_config::data& cfg,
                                 lfs::log& logger)
{
  const auto oid_data = split_oid(oid);

  if (!utils::directory_exists(cfg.file_directory + '/' + oid_data.parent_dir)) {
    if (utils::create_directory(cfg.file_directory + '/' + oid_data.parent_dir) == -1) {
	    std::stringstream ss;
	    ss << "Could not create directory " << oid_data.parent_dir;
	    logger.log_message(ss.str());
    }
  }

  if (!utils::directory_exists(cfg.file_directory + '/' + oid_data.parent_dir + '/' + oid_data.child_dir)) {
    if (utils::create_directory(cfg.file_directory + '/' + oid_data.parent_dir + '/' + oid_data.child_dir) == -1) {
	    std::stringstream ss;
	    ss << "Could not create directory " << oid_data.child_dir;
	    logger.log_message(ss.str());
    }
  }

  // This could be optimized so we do not call two times to split_oid.
  std::ofstream file {get_filesystem_path(cfg.file_directory, oid), std::ios::out | std::ios::binary};
  std::vector<unsigned char> raw_vec {raw.begin(), raw.end()};
  file.write(reinterpret_cast<char *>(&raw_vec.front()), sizeof(unsigned char) * raw_vec.size());
}

// Right now we only suport Basic auth.
// See RFC 2617, Section 2.
bool lfs::auth_ok(const request_t& request, const server_config::data& cfg)
{
  const auto auth = request.get_header_value("Authorization");
  const std::string prefix {"Basic "};

  if (auth.empty()) {
    return false;
  }

  if (auth.find(prefix) == std::string::npos) {
    return false;
  }

  const auto credentials = parse_b64_auth(auth, prefix);

  if (credentials.user.empty()) {
    return false;
  }

  return authenticate(credentials, cfg);
}

// AUTH will be something like this: "Basic dGVtcDp0ZW1w"
// PREFIX is Basic.
lfs::user_data lfs::parse_b64_auth(const std::string& auth, const std::string& prefix)
{
  const auto encoded_auth = auth.substr(prefix.length());

  if (encoded_auth.empty()) {
    return {"", ""};
  }

  const auto decoded_auth = base64_decode(encoded_auth);
  const auto colon_pos = decoded_auth.find(':');

  return {decoded_auth.substr(0, colon_pos), decoded_auth.substr(colon_pos + 1)};
}

bool lfs::authenticate(const lfs::user_data& credentials, const server_config::data& cfg)
{
  return credentials.user == cfg.user && credentials.passwd == cfg.passwd;
}

std::string lfs::get_href(const std::string& protocol, const std::string& host)
{
  return protocol + "://" + host;
}
