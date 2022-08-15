#pragma once

#include <exception>
#include <sstream>
#include <string>
#include <vector>

#include "json11.hpp"

namespace lfs {

class json_parse_error : public std::exception {
  std::string err_;

 public:
  json_parse_error(const std::string &err) : err_{err} {
  }

 public:
  const char *what() const noexcept {
    return err_.c_str();
  }
};

class json {
 public:
  json() {
    value = json11::Json();
  }

  json(const std::string &body) {
    std::string err;
    value = parse(body, err);

    if (!err.empty()) {
      std::stringstream ss;
      ss << "Error parsing JSON: " << err;
      throw json_parse_error{ss.str()};
    }
  }

  ~json() = default;

 public:
  json11::Json parse(const std::string &body, std::string &err) const {
    return json11::Json::parse(body, err);
  }

  auto get_string_value(const std::string &key) const {
    return value[key].string_value();
  }

  auto get_int_value(const std::string &key) const {
    return value[key].int_value();
  }

  auto get_boolean_value(const std::string &key) const {
    return value[key].bool_value();
  }

  auto get_array_items(const std::string &key) const {
    return value[key].array_items();
  }

  auto get_value() const {
    return value;
  }

  auto dump() const {
    return value.dump();
  }

 private:
  json11::Json value;
};

}  // namespace lfs
