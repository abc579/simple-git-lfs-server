#include <string>

#include "lfs.h"
#include "json11.hpp"

lfs::json_t lfs::parse_json(const std::string& j, std::string& err) {
  return json11::Json::parse(j, err);
}
