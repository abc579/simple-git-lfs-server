#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <iostream>

#include "util.h"

size_t util::get_file_size(const std::string& path) {
  if (!can_open(path)) {
    std::cerr << "get_file_size(): Could not open file " << path;
    return 0;
  }

  std::ifstream file(path, std::ios::in | std::ios::binary);
  file.unsetf(std::ios::skipws);
  file.seekg(0, std::ios::end);
  auto file_size = file.tellg();

  return file_size;
}
