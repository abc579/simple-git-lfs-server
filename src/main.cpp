#include <cstdlib>
#include <iostream>
#include <memory>

#include "httplib.h"
#include "get_storage_option.h"
#include "httplib/httplib_lfs_server.h"
#include "server/config.h"
#include "server/ilfs_server.h"

int main() {
  try {
    server::config cfg;
    std::unique_ptr<storage::istorage> storage_opt =
        get_storage_option(cfg.storage_option(), cfg.file_directory());
    std::unique_ptr<lfs::ilfs_server> sv =
        std::make_unique<lfs::httplib_lfs_server>(cfg, *storage_opt);
    sv->listen();
  } catch (const server::config_error& err) {
    std::cerr << "Config_error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const std::exception& err) {
    std::cerr << "Unhandled exception: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
