#include <cstdlib>
#include <iostream>

#include "server_config.h"
#include "httplib.h"
// #include "simple_git_lfs.h"

int main()
{
    try {
	server_config::data cfg;
	server_config::init(cfg);
	
	// Avoid significant C I/O overhead: we are not going to use C I/O and
	// we still care about performance.
	std::ios::sync_with_stdio(false);
	
	httplib::Server server;
	
    } catch (const std::exception& e) {
	std::cerr << e.what() << std::endl;
	return EXIT_FAILURE;
    }

    // server.Post("/objects/batch", sgls::batch_request_handler);
    // server.Get(R"(/objects/[a-zA-Z0-9]*$)", sgls::download_handler);
    // server.Put(R"(/objects/[a-zA-Z0-9]*$)", sgls::upload_handler);
    
    // server.listen(config::host, config::port);

    return EXIT_SUCCESS;
}
