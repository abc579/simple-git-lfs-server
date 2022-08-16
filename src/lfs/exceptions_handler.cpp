#include <exception>

#include "lfs.h"

// @NOTE(lev): the API does not specify clearly what to do if something like
// parsing a JSON goes bad. I have tried the reference Go server and they just
// return the following: { "objects" : "null }. We will do the same thing
// for now; notice that all exceptions that occur while the server is running
// will get here.
void lfs::exceptions_handler(const request_t&, response_t& response,
                             std::exception_ptr) {
  response.status = static_cast<int>(http_response_codes::ok);
  response.set_content(R"({"objects":null})", CONTENT_TYPE_LFS);
}
