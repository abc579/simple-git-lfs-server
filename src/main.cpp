#include <cstdlib>
#include <iostream>

#include "httplib.h"
#include "server/config.h"
#include "server/httplib/httplib_lfs_server.h"
#include "storage/get_storage_option.h"

int main() {
  // Since we are not using any C I/O and we still care about performance,
  // we are going to disable sync with these streams.
  std::ios::sync_with_stdio(false);
  try {
    server::config cfg;
    auto storage_opt =
        get_storage_option(cfg.storage_option(), cfg.file_directory());
    httplib::SSLServer hserver(cfg.cert().c_str(), cfg.key().c_str());
    lfs::httplib_lfs_server sv(cfg, *storage_opt, hserver);
    sv.listen();
  } catch (const server::config_error& err) {
    std::cerr << "Config_error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
