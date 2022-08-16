#pragma once

#include <fstream>
#include <string>

namespace logger {

struct logger_error : public std::exception {
  std::string msg;

  explicit logger_error(const std::string& e) : msg{e} {
  }

  const char* what() const noexcept {
    return msg.c_str();
  }
};

struct logger {
  std::ofstream file;

  logger();
  void log_message(const std::string&);
};
}  // namespace logger
