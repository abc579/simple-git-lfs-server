#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "httplib.h"
#include "server.h"
#include "lfs.h"
#include "json11.hpp"

using request_t = httplib::Request;
using response_t = httplib::Response;
using json_t = json11::Json;
using json_array_t = json11::Json::array;

void test_json_malformed() {
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

  server::data cfg;
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

void test_json_lacks_oid() {
  std::cout << "Testing test_json_lacks_oid...";

  const auto j = R"({
    "operation": "download",
    "transfers": [ "basic" ],
    "objects": [
     {
       "size": 123
     }
     ]})";

  server::data cfg;
  response_t response;
  request_t request;

  request.body = j;

  try {
    request.set_header("Authorization", "Basic test");
    lfs::batch_request_handler(request, response, cfg);
    assert(true);

    const auto new_response = response.body;
    std::string err;
    const auto json = lfs::parse_json(new_response, err);
    const auto object_array = json["objects"].array_items();

    for (const auto& ob : object_array) {
      assert(ob["oid"].string_value().empty());
    }

  } catch (const lfs::json_parse_error&) {
    assert(false);
  }

  std::cout << "ok" << std::endl;
}

void test_json_lacks_size() {
  std::cout << "Testing test_json_lacks_size()...";

  const auto j = R"({
    "operation": "download",
    "transfers": [ "basic" ],
    "objects": [
     {
       "oid": "5f70bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10ace3c6ef"
     }
     ]})";

  server::data cfg{"temp", "temp", "temp", 8080, "temp"};

  std::system("mkdir -p temp/5f/70/");
  std::system("fallocate -l 1KiB "
              "temp/5f/70/"
              "bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10ace3c6ef");

  response_t response;
  request_t request;

  request.body = j;

  try {
    request.set_header("Authorization", "Basic test");
    lfs::batch_request_handler(request, response, cfg);
    assert(true);

    const auto new_response = response.body;
    std::string err;
    const auto json = lfs::parse_json(new_response, err);
    const auto object_array = json["objects"].array_items();

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

void test_json_operation_unknown() {
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

  server::data cfg{"temp", "temp", "temp", 8080, "temp"};

  std::system("mkdir -p temp/5f/70/");
  std::system("fallocate -l 1KiB "
              "temp/5f/70/"
              "bf18a086007016e948b04aed3b82103a36bea41755b6cddfaf10ace3c6ef");

  response_t response;
  request_t request;

  request.body = j;

  try {
    request.set_header("Authorization", "Basic test");
    lfs::batch_request_handler(request, response, cfg);
    assert(true);

    const auto new_response = response.body;
    std::string err;
    const auto json = lfs::parse_json(new_response, err);
    const auto objects = json["objects"].array_items();
    const auto array = json_array_t(objects);

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

int main() {
  test_json_malformed();
  test_json_lacks_oid();
  test_json_lacks_size();
  test_json_operation_unknown();

  return EXIT_SUCCESS;
}
