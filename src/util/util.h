#pragma once

#include <string>

#include "server.h"

namespace util {

struct user_data {
  std::string user;
  std::string passwd;
};

struct oid_directory {
  std::string parent_dir;
  std::string child_dir;
  std::string oid;
};

bool can_open(const std::string &);
size_t get_file_size(const std::string &);
int create_directory(const std::string &);
bool directory_exists(const std::string &);
std::string get_url(const std::string &, const std::string &,
                    const std::string &, const std::string &);
std::string get_git_fs_path(const std::string &, const std::string &);
std::string get_oid_from_url(const std::string &);  // ?
void save_file(const std::string &, const std::string &, const server::data &);
oid_directory split_oid(const std::string &);
user_data parse_base64_auth(const std::string &, const std::string &);
bool authenticate(const user_data &, const server::data &);
}  // namespace util
