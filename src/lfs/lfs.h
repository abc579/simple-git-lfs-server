#pragma once

#include <string>

#include "httplib.h"
#include "json11.hpp"
#include "logger.h"
#include "server.h"
#include "util.h"

#define CONTENT_TYPE_LFS "application/vnd.git-lfs+json"
#define HASH_ALGO        "sha256"
#define ACCEPT_LFS       "application/vnd.git-lfs"
#define AUTH_ERROR \
  R"({ "message": "Credentials needed", "documentation_url": "https://lfs-server.com/docs/errors"})"
// @TODO: Implement --> 404
#define REPOSITORY_NOT_FOUND_ERROR \
  R"({ "message": "Not found", "documentation_url": "https://lfs-server.com/docs/errors"})"

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

struct error {
  std::string message;
  int code;
};

struct object {
  std::string oid;
  int size;
};

struct client_batch_request {
  std::vector<std::string> transfers;
  std::vector<object> objects;
  std::string operation;
  const std::string hash_algo{HASH_ALGO};
};

struct header {
  std::string authorization;
};

struct verify {
  header header_;
  std::string href;
  const int expires_in{86400};  // @TODO: How do I calculate this?
};

struct operation {
  header header_;
  std::string href;
  const int expires_in{86400};  // @TODO: How do I calculate this?
};

struct batch_response_object {
  operation actions_;
  verify verify_;
  error err_;
  object obj_;
  const bool authenticated{true};
};

struct batch_response {
  std::vector<batch_response_object> objects;
  const std::string transfer{"basic"};
  const std::string hash_algo{HASH_ALGO};
};

// Handy typedefs.
using request_t = httplib::Request;
using response_t = httplib::Response;
using json_t = json11::Json;
using json_object_t = json11::Json::object;
using json_array_t = json11::Json::array;

void exceptions_handler(const request_t &, response_t &, std::exception_ptr);
void batch_request_handler(const request_t &, response_t &,
                           const server::data &, logger::logger &);
void download_handler(const request_t &, response_t &,
                      const server::data &);
void upload_handler(const request_t &, response_t &,
                    const server::data &, logger::logger &);
void verify_handler(const request_t &, response_t &,
                    const server::data &);
bool process_auth(const request_t &, response_t &,
                  const server::data &);
bool parse_auth(const request_t &, util::user_data &);
json_t parse_json(const std::string &, std::string &);
void process_batch_request(const json_t &, response_t &,
                           const server::data &, const std::string &,
                           logger::logger &);
batch_response create_batch_response(const std::string &, const json_array_t &,
                                     const server::data &,
                                     const std::string &, logger::logger &);
std::string encode_batch_response(const batch_response &, const std::string &);
}  // namespace lfs
