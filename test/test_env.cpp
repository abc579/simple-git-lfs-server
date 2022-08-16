#include <cassert>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "server.h"

// Names of the environment variables.
#define LFS_HOST           "LFS_HOST"
#define LFS_ADMINUSER      "LFS_ADMINUSER"
#define LFS_ADMINPASSWD    "LFS_ADMINPASSWD"
#define LFS_FILE_DIRECTORY "LFS_FILE_DIRECTORY"
#define LFS_PORT           "LFS_PORT"
#define LFS_SCHEME         "LFS_SCHEME"
#define LFS_KEY            "LFS_KEY"
#define LFS_CERT           "LFS_CERT"

void run(const std::unordered_map<std::string, std::string>& env,
         const std::string& checking) {
  if (!env.empty()) {
    for (const auto& e : env) {
      setenv(e.first.c_str(), e.second.c_str(), 1);
    }
  }

  try {
    std::cout << "Checking " << checking << " environment variable...";
    server::data cfg = server::init();
    assert(false);
  } catch (const server::config_error&) {
    std::cout << " ok" << std::endl;
    assert(true);
  }
}

void check_lfs_host() {
  run({}, LFS_HOST);
}

void check_lfs_admin_user() {
  run({{LFS_HOST, "dummy"}}, LFS_ADMINUSER);
}

void check_lfs_admin_pass() {
  run({{LFS_HOST, "dummy"}, {LFS_ADMINUSER, "dummy"}}, LFS_ADMINPASSWD);
}

void check_lfs_file_directory() {
  run(
      {
          {LFS_HOST, "dummy"},
          {LFS_ADMINUSER, "dummy"},
          {LFS_ADMINPASSWD, "dummy"},
      },
      LFS_FILE_DIRECTORY);
}

void check_lfs_port() {
  run({{LFS_HOST, "dummy"},
       {LFS_ADMINUSER, "dummy"},
       {LFS_ADMINPASSWD, "dummy"},
       {LFS_FILE_DIRECTORY, "dummy"}},
      LFS_PORT);
}

void check_lfs_scheme() {
  run({{LFS_HOST, "dummy"},
       {LFS_ADMINUSER, "dummy"},
       {LFS_ADMINPASSWD, "dummy"},
       {LFS_FILE_DIRECTORY, "dummy"},
       {LFS_PORT, "8080"}},
      LFS_SCHEME);
}

void check_lfs_cert() {
  run({{LFS_HOST, "dummy"},
       {LFS_ADMINUSER, "dummy"},
       {LFS_ADMINPASSWD, "dummy"},
       {LFS_FILE_DIRECTORY, "dummy"},
       {LFS_PORT, "8080"},
       {LFS_SCHEME, "https"}},
      LFS_CERT);
}

void check_lfs_key() {
  run(
      {
          {LFS_HOST, "dummy"},
          {LFS_ADMINUSER, "dummy"},
          {LFS_ADMINPASSWD, "dummy"},
          {LFS_FILE_DIRECTORY, "dummy"},
          {LFS_PORT, "8080"},
          {LFS_SCHEME, "dummy"},
          {LFS_CERT, "dummy"},
      },
      LFS_KEY);
}

int main() {
  check_lfs_host();
  check_lfs_admin_user();
  check_lfs_admin_pass();
  check_lfs_file_directory();
  check_lfs_port();
  check_lfs_scheme();
  check_lfs_cert();
  check_lfs_key();

  std::cout << "All fine!" << std::endl;

  return EXIT_SUCCESS;
}
