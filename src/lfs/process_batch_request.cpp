#include <string>

#include "server.h"
#include "lfs.h"

void lfs::process_batch_request(const json_t& request, response_t& response,
                                const server::data& cfg,
                                const std::string& authorization) {
  const auto objects = request["objects"].array_items();

  // @NOTE(lev): API is silent about this, but since there is no
  // reasonable thing to do, we will throw an exception.
  if (objects.empty()) {
    throw json_parse_error{"`objects` key is mandatory."};
  }

  auto temp_op = request["operation"].string_value();

  // @NOTE(lev): again, the API does not tell what to do if the request
  // does not specify the operation or the operation is not download or upload.
  // In our case, we are going to assume that the client wants to download.
  if (temp_op.empty() || (temp_op != "download" && temp_op != "upload")) {
    temp_op = "download";
  }

  const auto operation = temp_op;

  batch_response br{
      create_batch_response(operation, objects, cfg, authorization)};

  response.set_header("Accept", ACCEPT_LFS);

  response.set_content(encode_batch_response(br, operation), CONTENT_TYPE_LFS);
  response.status = static_cast<int>(http_response_codes::ok);
}
