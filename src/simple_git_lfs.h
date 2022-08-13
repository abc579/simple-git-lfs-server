#pragma once

#include <string>

#include "httplib.h"
#include "json.h"
#include "json11.hpp"
#include "logger.h"
#include "server_config.h"

#define CONTENT_TYPE_LFS "application/vnd.git-lfs+json"
#define HASH_ALGO "sha256"
#define ACCEPT_LFS "application/vnd.git-lfs"

namespace lfs {

    enum class http_response_codes {
	ok = 200,
	auth_required_but_not_given = 401,
	validation_error = 422
    };

    enum class object_error_codes {
	not_found = 404
    };

    struct header_object {
	std::string authorization;
    };

    struct verify_object {
	header_object header;
	std::string href;
	const int expires_in {86400};
    };

    struct operation_object {
	header_object header;
	std::string href;
	const int expires_in {86400};
    };

    // "error": {
    //     "code": 404,
    //     "message": "Object does not exist"
    // }
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
	verify_object verify;
	error_object error;
	const bool authenticated {true};
    };

    struct batch_client_post_request {
	std::vector<batch_object> objects;
	std::vector<std::string> transfers;
	std::string operation;
    };

    // {
    //   "transfer": "basic",
    //   "objects": [
    //     {
    //       "oid": "1111111",
    //       "size": 123,
    //       "actions": {
    //         "download": {
    //           "href": "https://some-download.com",
    //           "expires_in" 3600,
    //         }
    //       }
    //     }
    //   ],
    //   "hash_algo": "sha256"
    // }
    struct batch_response {
	std::vector<batch_response_object> objects;
	const std::string transfer {"basic"};
    };

    using request_t = httplib::Request;
    using response_t = httplib::Response;

    using json_t = json11::Json;
    using json_object_t = json11::Json::object;
    using json_array_t  = json11::Json::array;

    // Handlers.
    void batch_request_handler(const request_t&, response_t&, const server_config::data&, lfs::log&);
    void download_handler(const request_t&, response_t&, const server_config::data&);
    void upload_handler(const request_t&, response_t&, const server_config::data&, lfs::log&);
    void verify_handler(const request_t&, response_t&, const server_config::data&);

    // Auth.
    bool auth_ok(const request_t&, const server_config::data&);

    void process_batch_request(const json&, response_t&, const server_config::data&, const std::string&,
			       lfs::log&);
    batch_response create_batch_response(const std::string&, const json_array_t&,
					 const server_config::data&, const std::string&,
					 lfs::log&);
    std::string encode_batch_response(const batch_response&, const std::string&);
    std::string get_href(const std::string&, const std::string&);
    std::string get_filesystem_path(const std::string&, const std::string&);
    std::string get_oid_from_url(const std::string&);
    void save_file_in_directory(const std::string&, const std::string&, const server_config::data&,
				lfs::log&);
    oid_directory split_oid(const std::string&);
    user_data parse_b64_auth(const std::string&, const std::string&);
    bool authenticate(const user_data&, const server_config::data&);
}
