#pragma once

#include "httplib.h"
#include "json11.hpp"

#include <unordered_map>
#include <fstream>
#include <sys/stat.h>
#include <iostream>
#include <iterator>
#include <algorithm>

namespace sgl {

const std::string content_type_lfs { "application/vnd.git-lfs+json" };
const std::string hash_algo { "sha256" };
const std::string file_directory { "/home/lev/repos/simple-git-lfs-server/files/" }; // @FIXME: use an environment variable or cfg file.
const std::string accept_lfs { "application/vnd.git-lfs" };
/**
 * @class link_object
 * @brief When the response does not involve an error, we have to create a link for the object to download. It is composed of these members.
 *
 * @TODO: do we also have a link when we are uploading? The API does not say anything.
 */
struct link_object {
    std::string href;
    std::unordered_map<std::string, std::string> header;
    std::string expires_at; // @TODO: Find a decent type for timestamps.
};

/**
 * @class error_object
 * @brief Errors per individual object.
 *
 */
struct error_object {
    int code;
    std::string message;
};

/**
 * @class batch_object
 * @brief Each object is identified by an OID and the corresponding size.
 *
 */
struct batch_object {
    std::string oid;
    int size;
};

/**
 * @class batch_response_object
 * @brief Each HTTP response is composed of these members. Check out `https://github.com/git-lfs/git-lfs/blob/main/docs/api/batch.md`.
 *
 * @note We either have an action or an error but not both at the same time.
 */
struct batch_response_object {
    batch_object object;
    std::unordered_map<std::string, link_object> actions;
    error_object error;
};

struct batch_client_post_request {
    std::string operation;
    std::vector<std::string> transfers;
    std::vector<batch_object> objects;
};

/**
 * @class batch_response
 * @brief The actual response that we are going to send to the LFS client.
 *
 */
struct batch_response {
    const std::string transfer { "basic" };
    std::vector<batch_response_object> objects;
};

class json_parse_error : public std::exception {
public:
    json_parse_error(const std::string &err)
        :error{err}
    {

    }
public:
    const char *what() const noexcept
    {
        return error.c_str();
    }
private:
    std::string error;
};

/**
 * @class json
 * @brief Wrapper for json objects. The reason why we do this is to ease maintenance if we change the library.
 *
 */
class json {
public:
    json()
    {
        value = json11::Json();
    }

    json(const std::string &body)
    {
        std::string err;
        value = parse(body, err);

        if (!err.empty()) {
            std::stringstream ss;
            ss << "Error parsing JSON: " << err;
            throw json_parse_error {ss.str()};
        }
    }

    ~json() = default;
public:
    json11::Json parse(const std::string &body, std::string &err) const {
        return json11::Json::parse(body, err);
    }

    auto get_string_value(const std::string &key) const {
        return value[key].string_value();
    }

    auto get_int_value(const std::string &key) const {
        return value[key].int_value();
    }

    auto get_boolean_value(const std::string &key) const {
        return value[key].bool_value();
    }

    auto get_array_items(const std::string &key) const {
        return value[key].array_items();
    }
public:
    auto get_value() const {
        return value;
    }

    auto dump() const {
        return value.dump();
    }
private:
    json11::Json value;
};

void batch_handler(const httplib::Request&, httplib::Response&);
void create_batch_response(const json&, httplib::Response&);
std::string encode_batch_response(const batch_response&);
bool can_open(const std::string &);
void download_handler(const httplib::Request&, httplib::Response&);
}
