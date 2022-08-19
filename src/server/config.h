#pragma once

#include <stdexcept>
#include <string>

namespace server {

class config_error : public std::exception {
 public:
  explicit config_error(const std::string& e) : error{e} {
  }

 public:
  const char* what() const noexcept {
    return error.c_str();
  }

 private:
  std::string error;
};

class config {
 public:
  config();
  ~config() = default;

 private:
  void ensure_env_vars_not_empty();
  void ensure_port_is_numeric();
  void ensure_key_cert_exists();
  std::string get_env_var_or_throw(const char*, const std::string&);

 public:
  auto host() const {
    return host_;
  }
  auto user() const {
    return user_;
  }
  auto passwd() const {
    return passwd_;
  }
  auto file_directory() const {
    return file_directory_;
  }
  auto get_upload_object_path() const {
    return upload_object_path_;
  }
  auto get_download_object_path() const {
    return download_object_path_;
  }
  auto get_verify_object_path() const {
    return verify_object_path_;
  }
  auto scheme() const {
    return scheme_;
  }
  auto cert() const {
    return cert_;
  }
  auto key() const {
    return key_;
  }
  auto port() const {
    return port_;
  }
  auto storage_option() const {
    return storage_option_;
  }

 private:
  std::string host_{""};
  std::string user_{""};
  std::string passwd_{""};
  std::string file_directory_{""};
  const std::string upload_object_path_{"/"};
  const std::string download_object_path_{"/objects/"};
  const std::string verify_object_path_{"/verify"};
  std::string scheme_{""};
  std::string cert_{""};
  std::string key_{""};
  int port_{0};
  std::string storage_option_{""};
};

}  // namespace server
