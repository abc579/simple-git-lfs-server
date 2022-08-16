#include <cassert>
#include <cstdlib>
#include <iostream>

#include "httplib.h"
#include "server.h"
#include "lfs.h"

using request = httplib::Request;
using response = httplib::Response;
using server_data = server::data;

void check_auth_ok() {
  std::cout << "Testing check_auth_ok()...";

  const server_data sd{"localhost", "flying", "around", 8080, "foo"};

  request req;
  response res;
  req.set_header("Authorization", "Basic Zmx5aW5nOmFyb3VuZA==");

  assert(lfs::process_auth(req, res, sd));

  std::cout << "ok" << std::endl;
}

void check_auth_nok() {
  std::cout << "Testing check_auth_nok()...";

  const server_data sd{"localhost", "flying", "around", 8080, "foo"};

  request req;
  response res;
  req.set_header("Authorization", "Basic Zmx5aCcCOmFyb3VuZA==");

  assert(!lfs::process_auth(req, res, sd));

  std::cout << "ok" << std::endl;
}

void check_incomplete_auth() {
  std::cout << "Testing check_incomplete_auth()...";

  const server_data sd{"localhost", "flying", "around", 8080, "foo"};

  request req;
  response res;

  req.set_header("Authorization", "Basic ");

  assert(!lfs::process_auth(req, res, sd));

  std::cout << "ok" << std::endl;
}

void check_auth_cut_half() {
  std::cout << "Testing check_auth_cut_half()...";

  const server_data sd{"localhost", "flying", "around", 8080, "foo"};

  request req;
  response res;

  req.set_header("Authorization", "Basic Zmx5");

  assert(!lfs::process_auth(req, res, sd));

  std::cout << "ok" << std::endl;
}

int main() {
  check_auth_ok();
  check_auth_nok();
  check_incomplete_auth();
  check_auth_cut_half();

  std::cout << "All fine!" << std::endl;

  return EXIT_SUCCESS;
}
