#include <string>

#include "util.h"

// https://testing.com/{oid} --> {oid}
std::string util::get_path_from_url(const std::string& url) {
  const auto pos = url.find_last_of('/');
  return url.substr(pos + 1);
}
