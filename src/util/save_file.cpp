#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "server.h"
#include "logger.h"
#include "util.h"

void util::save_file(const std::string& oid, const std::string& raw,
                     const server::data& cfg, logger::logger& logger) {
  const auto oid_data = split_oid(oid);

  if (!util::directory_exists(cfg.file_directory + '/' +
                               oid_data.parent_dir)) {
    if (util::create_directory(cfg.file_directory + '/' +
                                oid_data.parent_dir) == -1) {
      std::stringstream ss;
      ss << "Could not create directory " << oid_data.parent_dir;
      logger.log_message(ss.str());
    }
  }

  if (!util::directory_exists(cfg.file_directory + '/' + oid_data.parent_dir +
                               '/' + oid_data.child_dir)) {
    if (util::create_directory(cfg.file_directory + '/' + oid_data.parent_dir +
                                '/' + oid_data.child_dir) == -1) {
      std::stringstream ss;
      ss << "Could not create directory " << oid_data.child_dir;
      logger.log_message(ss.str());
    }
  }

  // This could be optimized so we do not call two times to split_oid.
  std::ofstream file{get_git_fs_path(cfg.file_directory, oid),
                     std::ios::out | std::ios::binary};
  std::vector<unsigned char> raw_vec{raw.begin(), raw.end()};
  file.write(reinterpret_cast<char*>(&raw_vec.front()),
             sizeof(unsigned char) * raw_vec.size());
}
