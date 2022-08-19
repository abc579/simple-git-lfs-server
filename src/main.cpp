#include <cstdlib>
#include <iostream>
#include <memory>

#include "httplib.h"
#include "server/config.h"
#include "server/lfs_server.h"
#include "storage/local/local_storage.h"

int main() {
  using namespace server;
  try {
    config cfg;
    httplib::SSLServer ssl_server(cfg.cert().c_str(), cfg.key().c_str());
    // @TODO: Check for option specified in env. variable. That is,
    // we need a switch statement to decide which option we are going
    // to use.
    std::unique_ptr<storage::storage> storage_option =
      std::make_unique<storage::local_storage>(cfg.file_directory());
    lfs_server sv(cfg, ssl_server, *storage_option);
    sv.listen();
  } catch (const config_error& err) {
    std::cerr << "Config_error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const std::exception& err) {
    std::cerr << "Unhandled exception: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
