#pragma once

#include <exception>
#include <string>

namespace server_config {

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

struct data {
  std::string host{""};
  std::string user{""};
  std::string passwd{""};
  int port{0};
  std::string file_directory{""};
  const std::string upload_object_path{"/"};
  const std::string download_object_path{"/objects/"};
  const std::string verify_object_path{"/verify"};
  std::string scheme{""};
  std::string cert{""};
  std::string key{""};
};

data init();
}  // namespace server_config
