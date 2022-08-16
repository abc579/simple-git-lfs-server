#include "logger.h"

#include <string>

logger::logger::logger() {
  file.open("lfs.log", std::ios::out | std::ios::trunc);

  if (!file) {
    throw logger_error{"Could not open file for logging."};
  }
}

void logger::logger::log_message(const std::string& msg) {
  file << msg;
  file.flush();
}
