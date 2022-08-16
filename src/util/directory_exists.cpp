#include <sys/stat.h>

#include <string>

#include "util.h"

bool util::directory_exists(const std::string& path) {
  struct stat s;

  stat(path.c_str(), &s);

  if (S_ISDIR(s.st_mode)) {
    return true;
  }

  return false;
}
