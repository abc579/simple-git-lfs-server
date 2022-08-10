#pragma once

#include <exception>
#include <string>

namespace server_config {

    class config_error : public std::exception {
	std::string error;
    public:
	config_error(const std::string& e)
	    :error {e} {}
    public:
	const char* what() const noexcept { return error.c_str(); }
    };

    struct data {
	const char* host {""};
	const char* user {""};
	const char* passwd {""};
	int port {0};
	const char* file_directory {""};
    };

    void init(data&);
}
