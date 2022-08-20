#include "get_storage_option.h"

#include "google/google_storage.h"
#include "local/local_storage.h"

std::unique_ptr<storage::istorage> get_storage_option(
    const std::string& option, const std::string& file_directory) {
  if (option == "google") {
    return std::make_unique<storage::google_storage>(file_directory);
  }

  return std::make_unique<storage::local_storage>(file_directory);
}
