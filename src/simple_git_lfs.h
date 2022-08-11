#pragma once

#include <string>

#include "httplib.h"
#include "json.h"
#include "server_config.h"

namespace lfs {

    const std::string content_type_lfs {"application/vnd.git-lfs+json"};
    const std::string hash_algo {"sha256"};
    const std::string accept_lfs {"application/vnd.git-lfs"};
    
    enum class http_response_codes {
	ok = 200,
	auth_required_but_not_given = 401,
	validation_error = 422
    };
    
    enum class object_error_codes {
	not_found = 404
    };

    // "download": {
    //       "href": "https://some-download.com",
    //       "header": {
    //         "Key": "value"
    //       },
    //       "expires_at": "2016-11-10T15:29:07Z"
    // }
    struct operation_object {
	std::string href;
	const std::string expires_in {"3600"};
    };

    // "error": {
    //     "code": 404,
    //     "message": "Object does not exist"
    //   }
    struct error_object {
	std::string message;
	int code;
    };

    // {
    //   "oid": "1111111",
    //   "size": 123,
    //   ...
    // }
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
    
    struct batch_response_object {
	batch_object object;
	operation_object actions;
	error_object error;
    };

    struct batch_client_post_request {
	std::string operation;
	std::vector<std::string> transfers;
	std::vector<batch_object> objects;
    };

    // {
    //   "transfer": "basic",
    //   "objects": [
    //     {
    //       "oid": "1111111",
    //       "size": 123,
    //       "authenticated": true,
    //       "actions": {
    //         "download": {
    //           "href": "https://some-download.com",
    //           "header": {
    //             "Key": "value"
    //           },
    //           "expires_at": "2016-11-10T15:29:07Z",
    //           "expires_in" 3600,
    //         }
    //       }
    //     }
    //   ],
    //   "hash_algo": "sha256"
    // }
    struct batch_response {
	const std::string transfer {"basic"};
	std::vector<batch_response_object> objects;
    };

    using request_t = httplib::Request;
    using response_t = httplib::Response;

    // Handlers.
    void batch_request_handler(const request_t&, response_t&, const server_config::data&);
    void download_handler(const request_t&, response_t&, const server_config::data&);
    void upload_handler(const request_t&, response_t&, const server_config::data&);

    // Utility functions.
    void create_batch_response(const json&, response_t&, const server_config::data&);
    std::string encode_batch_response(const batch_response&, const std::string&);
    bool can_open(const std::string&);
    size_t get_file_size(const std::string&);
    int create_directory(const std::string&);
    std::string get_filesystem_path(const std::string&, const std::string&);
    std::string get_oid_from_url(const std::string&);
    void save_file_in_directory(const std::string&, const std::string&, const server_config::data&);
    oid_directory split_oid(const std::string&);
    bool directory_exists(const std::string&);
}
