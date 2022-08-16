#pragma once

#include <string>
#include <memory>

#include "httplib.h"
#include "logger.h"

namespace server {

using ssl_server = httplib::SSLServer;

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

class lfs_server {
 public:
  lfs_server(const data&, ssl_server&, logger::logger&);

 public:
  void setup_listeners();
  void listen();

 private:
  ssl_server *server_;
  data cfg_;
  logger::logger *log_;
};

}  // namespace server
