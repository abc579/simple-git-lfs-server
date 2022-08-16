#include <string>

#include "lfs.h"
#include "util.h"

bool lfs::parse_auth(const request_t& request, util::user_data& credentials) {
  const auto auth = request.get_header_value("Authorization");
  const std::string prefix{"Basic "};

  if (auth.empty()) {
    return false;
  }

  if (auth.find(prefix) == std::string::npos) {
    return false;
  }

  // @TODO: There is something wrong with having both parse separated...
  credentials = util::parse_base64_auth(auth, prefix);

  if (credentials.user.empty()) {
    return false;
  }

  return true;
}
