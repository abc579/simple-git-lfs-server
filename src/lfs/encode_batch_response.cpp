#include <string>

#include "lfs.h"

#include <string>

using namespace lfs;

void encode_upload_batch_response(const lfs::batch_response& br,
                                  json_array_t& object_array) {
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

void encode_download_batch_response(const lfs::batch_response& br,
                                    json_array_t& object_array) {
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

std::string lfs::encode_batch_response(const batch_response& br,
                                       const std::string& operation) {
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
