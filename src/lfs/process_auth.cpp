#include "lfs.h"

#include "util.h"
#include "server.h"

bool lfs::process_auth(const request_t& request, response_t& response,
                       const server::data& cfg) {
  util::user_data credentials;

  if (!parse_auth(request, credentials) || !authenticate(credentials, cfg)) {
    response.status = static_cast<int>(http_response_codes::auth_required);
    response.set_content(AUTH_ERROR, CONTENT_TYPE_LFS);
    return false;
  }

  return true;
}
