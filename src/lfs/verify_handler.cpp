#include "lfs.h"
#include "server.h"
#include "util.h"
#include <string>

void lfs::verify_handler(const request_t& request, response_t& response,
                         const server::data& cfg) {
  std::string msg;
  const auto object = parse_json(request.body, msg);

  if (!msg.empty()) {
    throw json_parse_error{"verify_handler(): Error parsing JSON."};
  }

  const auto oid = object["oid"].string_value();

  if (!util::can_open(util::get_git_fs_path(cfg.file_directory, oid))) {
    response.status = static_cast<int>(object_error_codes::not_found);
  } else {
    response.status = static_cast<int>(http_response_codes::ok);
  }
}
