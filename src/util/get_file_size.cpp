#include <fstream>
#include <ios>
#include <sstream>
#include <string>

#include "logger.h"
#include "util.h"

size_t util::get_file_size(const std::string& path, logger::logger& logger) {
  if (!can_open(path)) {
    std::stringstream ss;
    ss << "get_file_size(): Could not open file " << path;
    logger.log_message(ss.str());
    return 0;
  }

  std::ifstream file(path, std::ios::in | std::ios::binary);
  file.unsetf(std::ios::skipws);
  file.seekg(0, std::ios::end);
  auto file_size = file.tellg();

  return file_size;
}
