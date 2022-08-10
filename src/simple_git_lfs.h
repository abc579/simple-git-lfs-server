#pragma once

#include "httplib.h"
#include "json.h"
#include "server.h"

#include <unordered_map>
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <sys/stat.h>
#include <cstdint>

namespace sgls {

    const std::string content_type_lfs {"application/vnd.git-lfs+json"};
    const std::string hash_algo {"sha256"};
    const std::string file_directory {"/home/lev/repos/simple-git-lfs-server/files"};
    const std::string accept_lfs {"application/vnd.git-lfs"};
    const std::string download_object_link {"http://localhost:8080/objects/"};
    const std::string upload_object_link {"http://localhost:8080/objects/"};

    enum class http_response_codes {
	ok = 200,
	auth_required_but_not_given = 401
    };
    
    enum class object_error_codes {
	not_found = 404
    };
    
    struct link_object {
	std::string href;
	const std::string expires_in {"3600"};
    };

    struct error_object {
	std::string message;
	int code;
    };

    struct batch_object {
	std::string oid;
	int size;
    };

    struct user_data {
	std::string user;
	std::string passwd;
    };

    struct oid_directory {
	std::string parent_dir;
	std::string child_dir;
	std::string oid;
    };
    
    using Actions = std::unordered_map<std::string, link_object>;
    
    struct batch_response_object {
	batch_object object;
	Actions actions;
	error_object error;
    };

    struct batch_client_post_request {
	std::string operation;
	std::vector<std::string> transfers;
	std::vector<batch_object> objects;
    };

    struct batch_response {
	const std::string transfer {"basic"};
	std::vector<batch_response_object> objects;
    };

    using request_t = httplib::Request;
    using response_t = httplib::Response;

    // Handlers.
    void batch_request_handler(const request_t&, response_t&);
    void download_handler(const request_t&, response_t&);
    void create_batch_response(const json&, response_t&);
    void upload_handler(const request_t&, response_t&);

    // Utility functions.
    std::string encode_batch_response(const batch_response&, const std::string&);
    bool can_open(const std::string&);
    size_t get_file_size(const std::string&);
    int create_directory(const std::string&);
    std::string get_filesystem_path(const std::string&); // TODO: Improve name.
    std::string get_oid_from_url(const std::string&);
    void save_file_in_directory(const std::string&, const std::string&);
    oid_directory split_oid(const std::string&);
    bool directory_exists(const std::string&);
}
