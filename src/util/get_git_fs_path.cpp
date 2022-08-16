#include <string>

#include "util.h"

std::string util::get_git_fs_path(const std::string& file_directory,
                                 const std::string& oid) {
  const auto oid_data = split_oid(oid);
  return file_directory + '/' + oid_data.parent_dir + '/' + oid_data.child_dir +
         '/' + oid_data.oid;
}
