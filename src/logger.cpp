#include "logger.h"

lfs::log::log()
{
  file.open("lfs.log", std::ios::out | std::ios::trunc);

  if (!file) {
    throw lfs::logger_error {"Could not open file for logging."};
  }
}

void lfs::log::log_message(const std::string& msg)
{
  file << msg;
  file.flush();
}
