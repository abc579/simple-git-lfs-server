#include <iostream>
#include <cstdlib>
#include <cassert>

#include "../src/server_config.h"
#include "../src/simple_git_lfs.h"
#include "../inc/httplib.h"

using request = httplib::Request;
using server_data = server_config::data;

void check_auth_ok()
{
  std::cout << "Testing check_auth_ok()...";

  const server_data sd {"localhost", "flying", "around", 8080, "foo"};

  request req;
  req.set_header("Authorization", "Basic Zmx5aW5nOmFyb3VuZA==");

  assert(lfs::auth_ok(req, sd));

  std::cout << "ok" << std::endl;
}

void check_auth_nok()
{
  std::cout << "Testing check_auth_nok()...";

  const server_data sd {"localhost", "flying", "around", 8080, "foo"};

  request req;
  req.set_header("Authorization", "Basic Zmx5aCcCOmFyb3VuZA==");

  assert(!lfs::auth_ok(req, sd));

  std::cout << "ok" << std::endl;
}

void check_incomplete_auth()
{
  std::cout << "Testing check_incomplete_auth()...";

  const server_data sd {"localhost", "flying", "around", 8080, "foo"};

  request req;
  req.set_header("Authorization", "Basic ");

  assert(!lfs::auth_ok(req, sd));

  std::cout << "ok" << std::endl;
}

void check_auth_cut_half()
{
  std::cout << "Testing check_auth_cut_half()...";

  const server_data sd {"localhost", "flying", "around", 8080, "foo"};

  request req;
  req.set_header("Authorization", "Basic Zmx5");

  assert(!lfs::auth_ok(req, sd));

  std::cout << "ok" << std::endl;
}

int main()
{
  check_auth_ok();
  check_auth_nok();
  check_incomplete_auth();
  check_auth_cut_half();

  std::cout << "All fine!" << std::endl;

  return EXIT_SUCCESS;
}
