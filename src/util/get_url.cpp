#include <string>

#include "util.h"

std::string util::get_url(const std::string& protocol, const std::string& host,
                          const std::string& port, const std::string& path) {
  return protocol + "://" + host + ':' + port + path;
}
