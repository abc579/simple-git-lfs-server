#include <cstdlib>
#include <iostream>

#include "server_config.h"
#include "httplib.h"
#include "simple_git_lfs.h"
#include "logger.h"

#define AUTH_ERROR R"({ "message": "Credentials needed", "documentation_url": "https://lfs-server.com/docs/errors"})"

int main()
{
    using httplib::Request;
    using httplib::Response;
    using httplib::Server;

    try {
	const server_config::data cfg {server_config::init()};
	lfs::log logger;

	// Avoid significant C I/O overhead: we are not going to use C I/O and
	// we still care about performance.
	std::ios::sync_with_stdio(false);

	Server server;

	// server.set_payload_max_length(1024 * 1024 * 512); // 512MB

	// @NOTE(lev): the API does not specify what to do if something like parsing
	// a JSON goes bad. I have tried the reference Go server and they just return
	// a JSON with the key `objects` to NULL.
	// We will do the same thing for now.
	server.set_exception_handler([](const auto&, auto& response, auto) {
	    response.status = static_cast<int>(lfs::http_response_codes::ok);
	    response.set_content(R"({"objects":null})", "application/vnd.git-lfs+json");
	});

	// @TODO(lev): get rid of repetition in else clause.
	server.Post("/objects/batch", [&cfg](const auto& request, auto& response) {
	    if (lfs::auth_ok(request, cfg)) {
		lfs::batch_request_handler(request, response, cfg);
	    } else {
		response.status = static_cast<int>(lfs::http_response_codes::auth_required_but_not_given);
		response.set_content(AUTH_ERROR, lfs::content_type_lfs);
	    }
	});

	server.Get(R"(/objects/[a-zA-Z0-9]*)", [&cfg](const auto& request, auto& response) {
	    if (lfs::auth_ok(request, cfg)) {
		lfs::download_handler(request, response, cfg);
	    } else {
		response.status = static_cast<int>(lfs::http_response_codes::auth_required_but_not_given);
		response.set_content(AUTH_ERROR, lfs::content_type_lfs);
	    }
	});

	server.Put(R"(/[a-zA-Z0-9]*)", [&cfg, &logger](const auto& request, auto& response) {
	    if (lfs::auth_ok(request, cfg)) {
		lfs::upload_handler(request, response, cfg, logger);
	    } else {
		response.status = static_cast<int>(lfs::http_response_codes::auth_required_but_not_given);
		response.set_content(AUTH_ERROR, lfs::content_type_lfs);
	    }
	});

	server.Post("/verify", [&cfg](const auto& request, auto& response) {
	    if (lfs::auth_ok(request, cfg)) {
		lfs::verify_handler(request, response, cfg);
	    } else {
		response.status = static_cast<int>(lfs::http_response_codes::auth_required_but_not_given);
		response.set_content(AUTH_ERROR, lfs::content_type_lfs);
	    }
	});

	server.listen(cfg.host, cfg.port);
    } catch (const server_config::config_error& e) {
	std::cerr << "Error in server configuration: " << e.what() << std::endl;
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
