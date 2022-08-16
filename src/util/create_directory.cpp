#include <sys/stat.h>

#include "util.h"

int util::create_directory(const std::string& path) {
  return mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
