#pragma once

#include <fstream>
#include <string>

namespace lfs {

  struct logger_error : public std::exception {
    std::string msg;

    logger_error(const std::string& e)
	    :msg {e} {}

    const char* what() const noexcept { return msg.c_str(); }
  };

  struct log {
    std::ofstream file;

    log();
    void log_message(const std::string&);
  };
}
