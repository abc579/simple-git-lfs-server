#include "google_storage.h"

using namespace storage;

google_storage::google_storage(const std::string &file_directory)
    : file_directory_{file_directory} {
}

void google_storage::write_file(const std::string &, const std::string &){

};

std::vector<unsigned char> google_storage::read_file(const std::string &) {
  return {};
};

bool google_storage::file_exists(const std::string &) {
  return false;
}

size_t google_storage::get_file_size(const std::string &) {
  return 0;
};
