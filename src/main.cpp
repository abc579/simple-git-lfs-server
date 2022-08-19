#include <cstdlib>
#include <iostream>
#include <memory>

#include "httplib.h"
#include "server/config.h"
#include "server/lfs_server.h"
#include "get_storage_option.h"

int main() {
  using namespace server;
  using namespace storage;
  try {
    config cfg;
    httplib::SSLServer ssl_server(cfg.cert().c_str(), cfg.key().c_str());
    auto so = get_storage_option(cfg.storage_option(), cfg.file_directory());
    lfs_server sv(cfg, ssl_server, *so);
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
