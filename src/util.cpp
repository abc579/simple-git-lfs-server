#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include "util.h"
#include "logger.h"

bool utils::can_open(const std::string& path)
{
  std::ifstream ifs (path, std::ios::in | std::ios::binary);

  return ifs.is_open();
}

size_t utils::get_file_size(const std::string& path, lfs::log& logger)
{
  if (!can_open(path)) {
    std::stringstream ss;
    ss << "get_file_size(): Could not open file " << path;

    logger.log_message(ss.str());
    return 0;
  }

  std::ifstream file (path, std::ios::in | std::ios::binary);
  file.unsetf(std::ios::skipws);
  file.seekg(0, std::ios::end);
  auto file_size = file.tellg();

  return file_size;
}

int utils::create_directory(const std::string& path)
{
  return mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

bool utils::directory_exists(const std::string& path)
{
  struct stat s;

  stat(path.c_str(), &s);

  if (S_ISDIR(s.st_mode)) {
    return true;
  }

  return false;
}
