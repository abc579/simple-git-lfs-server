#pragma once

#include <exception>
#include <string>

#include "httplib.h"
#include "json11.hpp"
#include "config.h"
#include "logger.h"

#define CONTENT_TYPE_LFS "application/vnd.git-lfs+json"
#define HASH_ALGO        "sha256"
#define ACCEPT_LFS       "application/vnd.git-lfs"
#define AUTH_ERROR \
  R"({ "message": "Credentials needed", "documentation_url": "https://lfs-server.com/docs/errors"})"

namespace lfs {

class json_parse_error : public std::exception {
 public:
  explicit json_parse_error(const std::string &err) : err_{err} {
  }

  const char *what() const noexcept {
    return err_.c_str();
  }

 private:
  std::string err_;
};

enum class http_response_codes { ok = 200, auth_required = 401 };

enum class object_error_codes { not_found = 404 };

struct header_object {
  std::string authorization;
};

struct verify_object {
  header_object header;
  std::string href;
  const int expires_in{86400};
};

struct operation_object {
  header_object header;
  std::string href;
  const int expires_in{86400};
};

// "error": {
//     "code": 404,
//     "message": "Object does not exist"
// }
struct error_object {
  std::string message;
  int code;
};

// {
//   "oid": "1111111",
//   "size": 123,
//   ...
// }
struct batch_object {
  std::string oid;
  int size;
};

struct user_data {
  std::string user;
  std::string passwd;
};

struct oid_directory {
  std::string parent_dir;
  std::string child_dir;
  std::string oid;
};

struct batch_response_object {
  batch_object object;
  operation_object actions;
  verify_object verify;
  error_object error;
  const bool authenticated{true};
};

struct batch_client_post_request {
  std::vector<batch_object> objects;
  std::vector<std::string> transfers;
  std::string operation;
};

// {
//   "transfer": "basic",
//   "objects": [
//     {
//       "oid": "1111111",
//       "size": 123,
//       "actions": {
//         "download": {
//           "href": "https://some-download.com",
//           "expires_in" 3600,
//         }
//       }
//     }
//   ],
//   "hash_algo": "sha256"
// }
struct batch_response {
  std::vector<batch_response_object> objects;
  const std::string transfer{"basic"};
};

// Trying to reduce coupling a little bit.
using request_t = httplib::Request;
using response_t = httplib::Response;
using json_t = json11::Json;
using json_object_t = json11::Json::object;
using json_array_t = json11::Json::array;

// Handlers.
void handle_exceptions(const request_t &, response_t &, std::exception_ptr);
void batch_request_handler(const request_t &, response_t &,
                           const server::config::data &, lfs::log &);
void download_handler(const request_t &, response_t &,
                      const server::config::data &);
void upload_handler(const request_t &, response_t &,
                    const server::config::data &, lfs::log &);
void verify_handler(const request_t &, response_t &,
                    const server::config::data &);

// Auth.
bool process_auth(const request_t &, response_t &,
                  const server::config::data &);
bool authenticate(const user_data &, const server::config::data &);
bool parse_auth(const request_t &, user_data &);

json_t parse_json(const std::string &, std::string &);
void process_batch_request(const json_t &, response_t &,
                           const server::config::data &, const std::string &,
                           lfs::log &);
batch_response create_batch_response(const std::string &, const json_array_t &,
                                     const server::config::data &,
                                     const std::string &, lfs::log &);
std::string encode_batch_response(const batch_response &, const std::string &);
std::string get_href(const std::string &, const std::string &,
                     const std::string &, const std::string &,
                     const std::string & = "");
std::string get_filesystem_path(const std::string &, const std::string &);
std::string get_oid_from_url(const std::string &);
void save_file_in_directory(const std::string &, const std::string &,
                            const server::config::data &, lfs::log &);
oid_directory split_oid(const std::string &);
user_data parse_b64_auth(const std::string &, const std::string &);
void encode_upload_batch_response(const batch_response &, json_array_t &);
void encode_download_batch_response(const batch_response &, json_array_t &);
}  // namespace lfs