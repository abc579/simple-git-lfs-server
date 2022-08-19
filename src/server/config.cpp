#include "config.h"

#include <cstdlib>

#include "util.h"

using namespace server;
using namespace util;

config::config() {
  ensure_env_vars_not_empty();
  ensure_port_is_numeric();
  ensure_key_cert_exists();
  create_directory(file_directory_);
}

void config::ensure_env_vars_not_empty() {
  host_ = get_env_var_or_throw(
      "LFS_HOST", "The environment variable LFS_HOST is mandatory.");
  user_ = get_env_var_or_throw(
      "LFS_ADMINUSER", "The environment variable LFS_ADMINUSER is mandatory.");
  passwd_ = get_env_var_or_throw(
      "LFS_ADMINPASSWD",
      "The environment variable LFS_ADMINPASSWD is mandatory.");
  scheme_ = get_env_var_or_throw(
      "LFS_SCHEME", "The environment variable LFS_SCHEME is mandatory.");
  cert_ = get_env_var_or_throw(
      "LFS_CERT", "The environment variable LFS_CERT is mandatory.");
  key_ = get_env_var_or_throw("LFS_KEY",
                              "The environment variable LFS_KEY is mandatory.");
  file_directory_ = get_env_var_or_throw(
      "LFS_FILE_DIRECTORY",
      "The environment variable LFS_FILE_DIRECTORY is mandatory.");
  storage_option_ = get_env_var_or_throw(
      "LFS_STORAGE_OPTION",
      "The environment variable LFS_STORAGE_OPTION is mandatory.");
}

void config::ensure_port_is_numeric() {
  auto port_str = get_env_var_or_throw(
      "LFS_PORT", "The environment variable LFS_PORT is mandatory.");

  try {
    port_ = std::stoi(port_str);
  } catch (const std::invalid_argument&) {
    throw config_error{"Could not convert port string to number because it "
                       "contains a non-numeric character."};
  } catch (const std::out_of_range&) {
    throw config_error{"Could not convert port string to number because the "
                       "number might be too large."};
  }
}

std::string config::get_env_var_or_throw(const char* env,
                                         const std::string& error) {
  const char* e;
  if ((e = std::getenv(env)) == nullptr) {
    throw config_error{error};
  }
  return std::string(e);
}

void config::ensure_key_cert_exists() {
  if (!can_open(cert_)) {
    throw config_error{"The environment variable LFS_CERT points to a non "
                       "existant certificate."};
  }

  if (!can_open(key_)) {
    throw config_error{"The environment variable LFS_KEY points to a non "
                       "existant private key."};
  }
}
