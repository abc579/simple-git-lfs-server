#include <iostream>
#include <cstdlib>
#include <cassert>
#include <unordered_map>

#include "../src/server_config.h"

#define LFS_HOST "LFS_HOST"
#define LFS_ADMINUSER "LFS_ADMINUSER"
#define LFS_ADMINPASSWD "LFS_ADMINPASSWD"
#define LFS_FILE_DIRECTORY "LFS_FILE_DIRECTORY"
#define LFS_PORT "LFS_PORT"
#define LFS_SCHEME "LFS_SCHEME"

void run(const std::unordered_map<std::string, std::string>& env, const std::string& checking)
{
    if (!env.empty()) {
	for (const auto& e : env) {
	    setenv(e.first.c_str(), e.second.c_str(), 1);
	}
    }

    try {
	std::cout << "Checking " << checking << " environment variable...";
	server_config::data cfg = server_config::init();
	assert(false);
    } catch(server_config::config_error&) {
	std::cout << " ok" << std::endl;
	assert(true);
    }
}

void check_lfs_host()
{
    run({}, LFS_HOST);
}

void check_lfs_admin_user()
{
    run({{LFS_HOST, "dummy"}}, LFS_ADMINUSER);
}

void check_lfs_admin_pass()
{
    run({{LFS_HOST, "dummy"},
	 {LFS_ADMINUSER, "dummy"}},
	LFS_ADMINPASSWD);
}

void check_lfs_file_directory()
{
    run({{LFS_HOST, "dummy"},
	 {LFS_ADMINUSER, "dummy"},
	 {LFS_ADMINPASSWD, "dummy"},
	}, LFS_FILE_DIRECTORY);
}

void check_lfs_port()
{
    run({{LFS_HOST, "dummy"},
	 {LFS_ADMINUSER, "dummy"},
	 {LFS_ADMINPASSWD, "dummy"},
	 {LFS_FILE_DIRECTORY, "dummy"}
	}, LFS_PORT);
}

void check_lfs_scheme()
{
    run({{LFS_HOST, "dummy"},
	 {LFS_ADMINUSER, "dummy"},
	 {LFS_ADMINPASSWD, "dummy"},
	 {LFS_FILE_DIRECTORY, "dummy"},
	 {LFS_PORT, "8080"}
	}, LFS_SCHEME);
}

int main()
{
    check_lfs_host();
    check_lfs_admin_user();
    check_lfs_admin_pass();
    check_lfs_file_directory();
    check_lfs_port();
    check_lfs_scheme();

    std::cout << "All fine!" << std::endl;

    return EXIT_SUCCESS;
}
