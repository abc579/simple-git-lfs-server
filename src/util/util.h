#pragma once

#include <string>
#include "json11.hpp"

namespace util {

using json_t = json11::Json;

int create_directory(const std::string &);
bool can_open(const std::string &);
json_t parse_json(const std::string &, std::string &);
std::string get_path_from_url(const std::string &);
std::string get_url(const std::string &, const std::string &,
                    const std::string &, const std::string &);
}  // namespace util
