#pragma once

#include <exception>
#include <string>

#include "json11.hpp"

namespace lfs {

class json_parse_error : public std::exception {
 public:
  explicit json_parse_error(const std::string &err) : err_{err} {
  }

  const char *what() const noexcept {
    return err_.c_str();
  }
private:
    std::string err_;
};

}  // namespace lfs
