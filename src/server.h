#pragma once

#include "httplib.h"
#include <functional>
#include <string>
#include <memory>

namespace sgls {

    // @TODO: Perhaps member variables should be unique_ptr's.
    // This interface is not very good. We might need to change it.
    template<typename Server, typename Request, typename Response>
    struct http_server {
	Server& server;
	Request request;
	Response response;

	void get(std::function<void()> handler) {
	    handler();
	}
	
	void post(std::function<void()> handler) {
	    handler();
	}
	
	void listen(std::function<void()> handler) {
	    handler();
	}

	http_server(Server& s, Request req, Response res)
	    : server {s}
	    , request {req}
	    , response {res} {}
	     
	~http_server() {}
    };

} // namespace sgls
