#include <string>

#include "util.h"

std::string util::get_oid_from_url(const std::string& url) {
  const auto pos = url.find_last_of('/');

  return url.substr(pos + 1);
}
