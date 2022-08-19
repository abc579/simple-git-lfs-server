#pragma once

#include <string>
#include <vector>

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

enum class http_response_codes {
  ok = 200,
  auth_required = 401,
  internal_error = 500
};

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

}  // namespace lfs
