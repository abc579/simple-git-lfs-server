#include <cstdlib>
#include <iostream>
#include <string>
#include <cassert>
#include <fstream>

#include "../inc/httplib.h"
#include "../src/server_config.h"
#include "../src/simple_git_lfs.h"
#include "../src/json.h"
#include "json11.hpp"

using request_t = httplib::Request;
using response_t = httplib::Response;
using json_t = json11::Json;

void test_json_malformed()
{
    std::cout << "Testing test_json_malformed()...";

    const auto malformed = R"({
    "operation": "download"
    "transfers": [ "basic" ],
    "objects": [
     {
       "oid": "12345678",
       "size": 123
     },
     {
       "oid": "123456789",
       "size": 1234
     }
    ]})";

    server_config::data cfg;
    response_t response;
    request_t request;

    request.body = malformed;

    try {
	lfs::batch_request_handler(request, response, cfg);
	assert(false);
    } catch (const lfs::json_parse_error&) {
	assert(true);
    }

    std::cout << "ok" << std::endl;
}

void test_json_lacks_oid()
{
    std::cout << "Testing test_json_lacks_oid...";

    const auto j = R"({
    "operation": "download",
    "transfers": [ "basic" ],
    "objects": [
     {
       "size": 123
     }
     ]})";

    server_config::data cfg;
    response_t response;
    request_t request;

    request.body = j;

    try {
	lfs::batch_request_handler(request, response, cfg);
	assert(true);

	const auto new_response = response.body;
	const auto json = lfs::json(new_response);
	const auto object_array = json.get_array_items("objects");

	for (const auto& ob : object_array) {
	    assert(ob["oid"].string_value().empty());
	}

    } catch (const lfs::json_parse_error&) {
	assert(false);
    }

    std::cout << "ok" << std::endl;

}

void test_json_lacks_size()
{
    std::cout << "Testing test_json_lacks_size()...";

    const auto j = R"({
    "operation": "download",
    "transfers": [ "basic" ],
    "objects": [
     {
       "oid": "5f70bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10ace3c6ef"
     }
     ]})";

    server_config::data cfg {"temp", "temp", "temp", 8080, "temp"};

    std::system("mkdir -p temp/5f/70/");
    std::system("fallocate -l 1KiB temp/5f/70/bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10ace3c6ef");

    response_t response;
    request_t request;

    request.body = j;

    try {
	lfs::batch_request_handler(request, response, cfg);
	assert(true);

	const auto new_response = response.body;
	const auto json = lfs::json(new_response);
	const auto object_array = json.get_array_items("objects");

	for (const auto& ob : object_array) {
	    if (ob["size"].int_value() != 1024) {
		std::system("rm -rf temp");
		assert(false);
	    }
	}

    } catch (const lfs::json_parse_error&) {
	assert(false);
    }

    std::system("rm -rf temp");

    std::cout << "ok" << std::endl;
}

void test_json_operation_unknown()
{
    std::cout << "Testing test_json_operation_unknown()...";

    const auto j = R"({
    "operation": "interesting",
    "transfers": [ "basic" ],
    "objects": [
     {
       "oid": "5f70bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10ace3c6ef",
       "size": 1024
     }
     ]})";

    server_config::data cfg {"temp", "temp", "temp", 8080, "temp"};

    std::system("mkdir -p temp/5f/70/");
    std::system("fallocate -l 1KiB temp/5f/70/bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10ace3c6ef");

    response_t response;
    request_t request;

    request.body = j;

    try {
	lfs::batch_request_handler(request, response, cfg);
	assert(true);

	const auto new_response = response.body;
	const auto json = lfs::json(new_response);
	const auto objects = json.get_array_items("objects");
	const auto array = json11::Json::array(objects);

	for (const auto& o : objects) {
	    const auto actions = o["actions"];
	    assert(!actions["download"].dump().empty());
	}

    } catch (const lfs::json_parse_error&) {
	assert(false);
    }

    std::system("rm -rf temp");

    std::cout << "ok" << std::endl;
}

int main()
{
    test_json_malformed();
    test_json_lacks_oid();
    test_json_lacks_size();
    test_json_operation_unknown();

    return EXIT_SUCCESS;
}
