#pragma once

#include <utility>

#include "config.h"
#include "httplib.h"
#include "json11.hpp"
#include "lfs.h"
#include "storage.h"

namespace server {

// @TODO: Get rid of this displeasant coupling.
using ssl_server = httplib::SSLServer;
using request_t = httplib::Request;
using response_t = httplib::Response;
using json_t = json11::Json;
using json_object_t = json11::Json::object;
using json_array_t = json11::Json::array;

class lfs_server {
 public:
  lfs_server(config &, ssl_server &, storage::storage &);

 public:
  void listen();

  // Handlers.
 private:
  void setup_handlers();
  static void exceptions_handler(const request_t &, response_t &, std::exception_ptr);
  void batch_request_handler(const request_t &, response_t &);
  void download_handler(const request_t &, response_t &);
  void upload_handler(const request_t &, response_t &);
  void verify_handler(const request_t &, response_t &);

 private:
  void process_batch_request(const json_t &, const std::string&, response_t &);
  void create_download_batch_response(const json_array_t &, const std::string &,
                                      lfs::batch_response &);
  void create_upload_batch_response(const json_array_t &objects,
                                    const std::string &authorization,
                                    lfs::batch_response &br);
  lfs::batch_response create_batch_response(const std::string &,
                                            const json_array_t &,
                                            const std::string &);
  std::string encode_batch_response(const lfs::batch_response &,
                                    const std::string &);
  void encode_download_batch_response(const lfs::batch_response &,
                                      json_array_t &);
  void encode_upload_batch_response(const lfs::batch_response &,
                                    json_array_t &);
  // Auth.
 private:
  bool process_auth(const request_t &, response_t &);
  bool authenticate(const request_t &);
  std::pair<std::string, std::string> get_credentials_from_base64(
      const std::string &, const std::string &);

 private:
  ssl_server *server_;
  config *cfg_;
  storage::storage *storage_;
};

}  // namespace server
