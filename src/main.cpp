#include <cstdlib>
#include <iostream>

#include "httplib.h"
#include "server/server.h"

int main() {
  using ssl_server = httplib::SSLServer;
  
  try {
    const auto cfg = server::init();
    ssl_server server (cfg.cert.c_str(), cfg.key.c_str());
    server::lfs_server sv (cfg, server);
    sv.setup_listeners();
    sv.listen();
  } catch (const server::config_error& err) {
    std::cerr << "Config_error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const std::exception &err) {
    std::cerr << "Unhandled exception: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
