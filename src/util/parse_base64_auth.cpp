#include "base64.h"
#include "util.h"

util::user_data util::parse_base64_auth(const std::string& auth,
                                        const std::string& prefix) {
  const auto encoded = auth.substr(prefix.length());

  if (encoded.empty()) {
    return {"", ""};
  }

  const auto decoded = base64_decode(encoded);
  const auto colon_pos = decoded.find(':');

  return {decoded.substr(0, colon_pos), decoded.substr(colon_pos + 1)};
}
