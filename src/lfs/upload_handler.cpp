#include "server.h"
#include "lfs.h"
#include "logger.h"
#include "util.h"

void lfs::upload_handler(const request_t& request, response_t& response,
                         const server::data& cfg,
                         logger::logger& logger) {
  util::save_file(util::get_oid_from_url(request.target), request.body, cfg,
                  logger);
  response.status = static_cast<int>(http_response_codes::ok);
}
