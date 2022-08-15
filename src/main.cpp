#include <cstdlib>
#include <iostream>

#include "httplib.h"
#include "logger.h"
#include "server_config.h"
#include "simple_git_lfs.h"

int main() {
  using httplib::Request;
  using httplib::Response;
  using httplib::SSLServer;

  try {
    const server_config::data cfg{server_config::init()};
    lfs::log logger;

    // Avoid significant C I/O overhead: we are not going to use C I/O and
    // we still care about performance. We are not interested in keeping C's I/O
    // and C++ I/O synced, however, this has a caveat: we lose thread safety for
    // C++ streams.
    std::ios::sync_with_stdio(false);

    SSLServer server(cfg.cert.c_str(), cfg.key.c_str());

    // @NOTE(lev): the API does not specify what to do if something like parsing
    // a JSON goes bad. I have tried the reference Go server and they just
    // return the following: { "objects" : "null }. We will do the same thing
    // for now.
    server.set_exception_handler([](const auto&, auto& response, auto) {
      response.status = static_cast<int>(lfs::http_response_codes::ok);
      response.set_content(R"({"objects":null})", CONTENT_TYPE_LFS);
    });

    server.Post("/objects/batch",
                [&cfg, &logger](const auto& request, auto& response) {
                  if (lfs::process_auth(request, response, cfg)) {
                    lfs::batch_request_handler(request, response, cfg, logger);
                  }
                });

    server.Get(R"(/objects/[a-zA-Z0-9]*)",
               [&cfg](const auto& request, auto& response) {
                 if (lfs::process_auth(request, response, cfg)) {
                   lfs::download_handler(request, response, cfg);
                 }
               });

    server.Put(R"(/[a-zA-Z0-9]*)",
               [&cfg, &logger](const auto& request, auto& response) {
                 if (lfs::process_auth(request, response, cfg)) {
                   lfs::upload_handler(request, response, cfg, logger);
                 }
               });

    server.Post("/verify", [&cfg](const auto& request, auto& response) {
      lfs::verify_handler(request, response, cfg);
    });

    server.listen(cfg.host, cfg.port);
  } catch (const server_config::config_error& e) {
    std::cerr << "Error in server configuration: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
