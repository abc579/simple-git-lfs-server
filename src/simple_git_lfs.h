#pragma once

#include "httplib.h"
#include "json11.hpp"

namespace sgl {

struct batch_response;

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

class json {
public:
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
    json11::Json parse(const std::string &body, std::string &err) {
        return json11::Json::parse(body, err);
    }

    auto get_string_value(const std::string &key) {
        return value[key].string_value();
    }

    auto get_int_value(const std::string &key) {
        return value[key].int_value();
    }

    auto get_boolean_value(const std::string &key) {
        return value[key].bool_value();
    }
private:
    json11::Json value;
};

struct batch_object {
    std::string oid;
    size_t size;
};

struct batch_post_request {
    std::string operation;
    std::vector<std::string> transfers;
    std::vector<batch_object> objects;
};

void batch_handler(const httplib::Request&, httplib::Response&);
batch_response create_batch_response(const batch_post_request&);
}
