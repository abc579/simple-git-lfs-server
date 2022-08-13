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
	std::string host {""};
	std::string user {""};
	std::string passwd {""};
	int port {0};
	std::string file_directory {""};
	const std::string upload_object_path {"/"};
	const std::string download_object_path {"/objects/"};
	const std::string verify_object_path {"/"};
	std::string scheme {""};
    };

    data init();
}
