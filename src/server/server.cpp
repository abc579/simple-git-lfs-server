#include "server.h"

#include <fstream>

#include "httplib.h"
#include "lfs.h"
#include "util.h"

server::lfs_server::lfs_server(const data& cfg, ssl_server& sv)
    : cfg_{cfg} {
  server_ = &sv;
}

void server::lfs_server::setup_listeners() {
  server_->set_exception_handler(lfs::exceptions_handler);

  server_->Post("/objects/batch", [&](const auto& request, auto& response) {
    if (lfs::process_auth(request, response, cfg_)) {
      lfs::batch_request_handler(request, response, cfg_);
    }
  });

  server_->Get(R"(/objects/[a-zA-Z0-9]*)",
               [&](const auto& request, auto& response) {
                 if (lfs::process_auth(request, response, cfg_)) {
                   lfs::download_handler(request, response, cfg_);
                 }
               });

  server_->Put(R"(/[a-zA-Z0-9]*)", [&](const auto& request, auto& response) {
    if (lfs::process_auth(request, response, cfg_)) {
      lfs::upload_handler(request, response, cfg_);
    }
  });

  server_->Post("/verify", [&](const auto& request, auto& response) {
    lfs::verify_handler(request, response, cfg_);
  });
}

void server::lfs_server::listen() {
  server_->listen(cfg_.host, cfg_.port);
}

server::data server::init() {
  data c;

  const char* host;
  const char* user;
  const char* passwd;
  const char* file_directory;
  const char* port;
  const char* scheme;
  const char* cert;
  const char* key;

  // Check environment variables.
  host = std::getenv("LFS_HOST");

  if (host == nullptr) {
    throw config_error{"The environment variable LFS_HOST is mandatory."};
  }

  user = std::getenv("LFS_ADMINUSER");

  if (user == nullptr) {
    throw config_error{"The environment variable LFS_ADMINUSER is mandatory."};
  }

  passwd = std::getenv("LFS_ADMINPASSWD");

  if (passwd == nullptr) {
    throw config_error{
        "The environment variable LFS_ADMINPASSWD is mandatory."};
  }

  port = std::getenv("LFS_PORT");

  if (port == nullptr) {
    throw config_error{"The environment variable LFS_PORT is mandatory."};
  }

  scheme = std::getenv("LFS_SCHEME");

  if (scheme == nullptr) {
    throw config_error{"The environment variable LFS_SCHEME is mandatory."};
  }

  cert = std::getenv("LFS_CERT");

  if (cert == nullptr) {
    throw config_error{"The environment variable LFS_CERT is mandatory."};
  }

  key = std::getenv("LFS_KEY");

  if (key == nullptr) {
    throw config_error{"The environment variable LFS_KEY is mandatory."};
  }

  try {
    c.port = std::stoi(port);
  } catch (const std::invalid_argument&) {
    throw config_error{"Could not convert port string to number because it "
                       "contains a non-numeric character."};
  } catch (const std::out_of_range&) {
    throw config_error{"Could not convert port string to number because the "
                       "number might be too large."};
  }

  file_directory = std::getenv("LFS_FILE_DIRECTORY");

  if (file_directory == nullptr) {
    throw config_error{
        "The environment variable LFS_FILE_DIRECTORY is mandatory."};
  }

  c.host = host;
  c.user = user;
  c.passwd = passwd;
  c.file_directory = file_directory;
  c.scheme = scheme;
  c.cert = cert;
  c.key = key;

  // Check if the cert and key provided exist in the directory.
  if (!util::can_open(c.cert)) {
    throw config_error{"The environment variable LFS_CERT points to a non "
                       "existant certificate."};
  }

  if (!util::can_open(c.key)) {
    throw config_error{"The environment variable LFS_KEY points to a non "
                       "existant private key."};
  }

  // Create the directory where all files are going to be stored.
  util::create_directory(file_directory);

  return c;
}
