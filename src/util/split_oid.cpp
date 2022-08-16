#include <string>

#include "util.h"

util::oid_directory util::split_oid(const std::string& oid) {
  return {oid.substr(0, 2), oid.substr(2, 2), oid.substr(4)};
}
