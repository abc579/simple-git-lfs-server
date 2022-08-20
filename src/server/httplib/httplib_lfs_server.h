#pragma once

#include <map>
#include <memory>
#include <string>

#include "lfs.h"

namespace httplib {
class SSLServer;
struct Request;
struct Response;
}  // namespace httplib

namespace server {
class config;
}  // namespace server

namespace storage {
class istorage;
}  // namespace storage

namespace json11 {
class Json;
using object = std::map<std::string, Json>;
using array = std::vector<Json>;
}  // namespace json11

namespace lfs {

using json_t = json11::Json;
using json_object_t = json11::object;
using json_array_t = json11::array;

class httplib_lfs_server {
 public:
  httplib_lfs_server(server::config &, storage::istorage &, httplib::SSLServer&);
  ~httplib_lfs_server() = default;

 public:
  void listen();
  void stop();
  void set_batch_handler();
  void set_download_handler();
  void set_upload_handler();
  void set_verify_handler();
  void set_exception_handler();

 private:
  void process_batch_request(const json_t &, const std::string &,
                             httplib::Response &);
  void create_download_response(const json_array_t &, const std::string &,
                                batch_response &);
  void create_upload_response(const json_array_t &objects,
                              const std::string &authorization,
                              batch_response &br);
  batch_response create_batch_response(const std::string &,
                                       const json_array_t &,
                                       const std::string &);
  std::string encode_batch_response(const batch_response &,
                                    const std::string &);
  void encode_download_batch_response(const batch_response &, json_array_t &);
  void encode_upload_batch_response(const batch_response &, json_array_t &);
  bool process_auth(const httplib::Request &, httplib::Response &);
  bool authenticate(const httplib::Request &);
  std::pair<std::string, std::string> get_credentials_from_base64(
      const std::string &, const std::string &);

 private:
  httplib::SSLServer* server_;
  server::config *cfg_;
  storage::istorage *storage_;
};
};  // namespace lfs
