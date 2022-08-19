#include <fstream>
#include <ios>
#include <string>

#include "util.h"

bool util::can_open(const std::string& path) {
  std::ifstream ifs(path, std::ios::in | std::ios::binary);
  return ifs.is_open();
}
