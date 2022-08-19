#include "local_storage.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "util.h"

storage::local_storage::local_storage(const std::string& file_directory)
    : file_directory_{file_directory} {
}

void storage::local_storage::write_file(const std::string& oid,
                                        const std::string& raw) {
  std::ofstream file{file_directory_ + '/' + oid,
                     std::ios::out | std::ios::binary};
  file.write(reinterpret_cast<char*>(raw.front()),
             sizeof(unsigned char) * raw.size());
  file.close();
}

std::vector<unsigned char> storage::local_storage::read_file(
    const std::string& oid) {
  std::ifstream file(file_directory_ + '/' + oid,
                     std::ios::in | std::ios::binary);
  std::noskipws(file);
  std::vector<unsigned char> binary_file(std::istream_iterator<char>(file),
                                         std::istream_iterator<char>{});
  file.close();
  return binary_file;
}

bool storage::local_storage::file_exists(const std::string& oid) {
  return util::can_open(file_directory_ + '/' + oid);
}

size_t storage::local_storage::get_file_size(const std::string& oid) {
  std::ifstream file(file_directory_ + '/' + oid,
                     std::ios::in | std::ios::binary);
  file.unsetf(std::ios::skipws);
  file.seekg(0, std::ios::end);
  file.close();
  return file.tellg();
}
