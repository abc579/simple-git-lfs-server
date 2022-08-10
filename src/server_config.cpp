#include "server_config.h"

#include <cstdlib>
#include <iostream>
#include <exception>

void server_config::init(data& c)
{
    c.host = std::getenv("LFS_HOST");

    if (c.host == nullptr) {
	throw config_error {"The environment variable LFS_HOST is mandatory."};
    }
    
    c.user = std::getenv("LFS_ADMINUSER");

    if (c.user == nullptr) {
	throw config_error {"The environment variable LFS_ADMINUSER is mandatory."};
    }
    
    c.passwd = std::getenv("LFS_ADMINPASSWD");

    if (c.passwd == nullptr) {
	throw config_error {"The environment variable LFS_ADMINPASSWD is mandatory."};
    }

    const auto port_temp = std::getenv("LFS_PORT");

    if (port_temp == nullptr) {
	throw config_error {"The environment variable LFS_PORT is mandatory."};
    }
    
    try {
	c.port = std::stoi(port_temp);
    } catch (const std::invalid_argument&) {
	throw config_error {"Could not convert port string to number because it contains a non-numeric character."};
    } catch (const std::out_of_range&) {
	throw config_error {"Could not convert port string to number because the number might be too large."};
    }
    
    c.file_directory = std::getenv("LFS_FILE_DIRECTORY");

    if (c.file_directory == nullptr) {
	throw config_error {"The environment variable LFS_FILE_DIRECTORY is mandatory."};
    }
}
