#include <cassert>
#include <cstdlib>
#include <iostream>

#include "../inc/base64.h"

int main() {
  std::cout << "Testing test_base64()...";

  const std::string b64_string{"Zmx5aW5nOmFyb3VuZA=="};
  const auto result = base64_decode(b64_string);
  const std::string expected{"flying:around"};

  if (!(expected == result)) {
    assert(false);
  }

  std::cout << "ok!" << std::endl;

  return EXIT_SUCCESS;
}
