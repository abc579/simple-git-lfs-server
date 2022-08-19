#include <string>

#include "json11.hpp"

#include "util.h"

util::json_t util::parse_json(const std::string& j, std::string& err) {
  return json11::Json::parse(j, err);
}
