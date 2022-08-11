#include <cstdlib>
#include <iostream>

#include "server_config.h"
#include "httplib.h"
#include "simple_git_lfs.h"

int main()
{
    using httplib::Request;
    using httplib::Response;
    using httplib::Server;
  
    try {
	const server_config::data cfg {server_config::init()};
	
	// Avoid significant C I/O overhead: we are not going to use C I/O and
	// we still care about performance.
	std::ios::sync_with_stdio(false);
	
	Server server;

	// @NOTE(lev): the API does not specify what to do if something like parsing
	// a JSON goes bad. I have tried the Go server that they offer as an example
	// and they just return a JSON with the key `objects` to NULL.
	// We will do the same thing for now.
	server.set_exception_handler([](const auto&, auto& response, auto) {
	    response.status = 200;
	    response.set_content(R"({"objects":null})", "application/vnd.git-lfs+json");
	});
	
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
