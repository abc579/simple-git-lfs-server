#pragma once

#include <string>
#include "logger.h"

namespace utils {
  bool can_open(const std::string&);
  size_t get_file_size(const std::string&, lfs::log&);
  int create_directory(const std::string&);
  bool directory_exists(const std::string&);
}
