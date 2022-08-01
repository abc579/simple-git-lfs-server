#pragma once

#include "httplib.h"
#include "json11.hpp"

namespace sgl {

struct lfs_object {
    std::string oid;
    int size;
};

struct git_lfs_request {
    std::string operation;
    std::string transfers;
    /* @TODO: ref */
    std::vector<lfs_object> objects;
    std::string hash_algo;

};

void batch_handler(const httplib::Request&, httplib::Response&);
}
