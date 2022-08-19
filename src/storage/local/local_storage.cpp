#include "local_storage.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

#include "util.h"

storage::local_storage::local_storage(const std::string& file_directory)
    : file_directory_{file_directory} {
}

void storage::local_storage::write_file(const std::string& oid,
                                        const std::string& raw) {
  if (file_exists(oid)) {
    return;
  }
  std::ofstream file{file_directory_ + '/' + oid,
                     std::ios::out | std::ios::binary};
  if (!file) {
    std::stringstream ss;
    ss << "local_storage.write_file(): Error opening file: "
       << std::error_code{errno, std::generic_category()}.message();
    throw storage_exception{ss.str()};
  }
  file.write(reinterpret_cast<char*>(raw.front()),
             sizeof(unsigned char) * raw.size());
  if (!file.good()) {
    std::stringstream ss;
    ss << "local_storage.write_file(): Error while writing file: "
       << std::error_code{errno, std::generic_category()}.message();
    throw storage_exception{ss.str()};
  }
}

std::string storage::local_storage::read_file(const std::string& oid) {
  std::ifstream file(file_directory_ + '/' + oid,
                     std::ios::in | std::ios::binary);
  if (!file) {
    std::stringstream ss;
    ss << "local_storage.read_file(): Error reading file: "
       << std::error_code{errno, std::generic_category()}.message();
    throw storage_exception{ss.str()};
  }
  std::noskipws(file);
  std::string binary_file(std::istream_iterator<char>(file),
                          std::istream_iterator<char>{});
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
