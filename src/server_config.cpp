#include "server_config.h"
#include "simple_git_lfs.h"

#include <cstdlib>
#include <iostream>
#include <exception>

server_config::data server_config::init()
{
    data c;

    const char* host;
    const char* user;
    const char* passwd;
    const char* file_directory;
    const char* port;
    const char* scheme;

    host = std::getenv("LFS_HOST");

    if (host == nullptr) {
	throw config_error {"The environment variable LFS_HOST is mandatory."};
    }

    user = std::getenv("LFS_ADMINUSER");

    if (user == nullptr) {
	throw config_error {"The environment variable LFS_ADMINUSER is mandatory."};
    }

    passwd = std::getenv("LFS_ADMINPASSWD");

    if (passwd == nullptr) {
	throw config_error {"The environment variable LFS_ADMINPASSWD is mandatory."};
    }

    port = std::getenv("LFS_PORT");

    if (port == nullptr) {
	throw config_error {"The environment variable LFS_PORT is mandatory."};
    }

    scheme = std::getenv("LFS_SCHEME");

    if (scheme == nullptr) {
	throw config_error {"The environment variable LFS_SCHEME is mandatory."};
    }

    try {
	c.port = std::stoi(port);
    } catch (const std::invalid_argument&) {
	throw config_error {"Could not convert port string to number because it contains a non-numeric character."};
    } catch (const std::out_of_range&) {
	throw config_error {"Could not convert port string to number because the number might be too large."};
    }

    file_directory = std::getenv("LFS_FILE_DIRECTORY");

    if (file_directory == nullptr) {
	throw config_error {"The environment variable LFS_FILE_DIRECTORY is mandatory."};
    }

    c.host = host;
    c.user = user;
    c.passwd = passwd;
    c.file_directory = file_directory;
    c.scheme = scheme;

    return c;
}
