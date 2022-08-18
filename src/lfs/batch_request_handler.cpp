#include "lfs.h"
#include "server.h"

void lfs::batch_request_handler(const request_t& request, response_t& response,
                                const server::data& cfg) {
  std::string err;
  auto req = lfs::parse_json(request.body, err);

  if (!err.empty()) {
    throw json_parse_error{"batch_request_handler(): Bad JSON format."};
  }

  const auto auth = request.headers.find("Authorization")->second;

  process_batch_request(req, response, cfg, auth);

  response.status = static_cast<int>(http_response_codes::ok);
}
