#include <cstdlib>
#include <iostream>

#include "server_config.h"
#include "httplib.h"
#include "simple_git_lfs.h"

int main()
{
    try {
	server_config::data cfg;
	server_config::init(cfg);
	
	// Avoid significant C I/O overhead: we are not going to use C I/O and
	// we still care about performance.
	std::ios::sync_with_stdio(false);
	
	httplib::Server server;
	
	server.Post("/objects/batch", [&cfg](const auto& request, auto& response) {
	    lfs::batch_request_handler(request, response, cfg);
	});

	server.Get(R"(/objects/[a-zA-Z0-9]*$)", [&cfg](const auto& request, auto& response) {
	    lfs::download_handler(request, response, cfg);
	});

	server.Put(R"(/objects/[a-zA-Z0-9]*$)", [&cfg](const auto& request, auto& response) {
	    lfs::upload_handler(request, response, cfg);
	});
	
	server.listen(cfg.host, cfg.port);
    } catch (const std::exception& e) {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
