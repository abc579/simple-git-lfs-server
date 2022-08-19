#pragma once

#include <stdexcept>
#include <string>

namespace storage {

// @TEMP: Let's see if we can use exceptions to handle storage errors.
class storage_exception : public std::exception {
  std::string err;

 public:
  storage_exception(const std::string& error) : err{error} {
  }

 public:
  const char* what() const noexcept {
    return err.c_str();
  }
};

// Using an interface to ease maintenance.
//
// Initially, we only had local storage, but since this can change we
// needed to make it extensible.  The idea here is simple: if we need
// a new way of storing objects, say we use Amazon, then we only have
// to create a new class that implements this interface; that is it.
//
// I have tried using functions instead of classes but since
// I am unexperienced it turned out bad; it was not clean.
// This approach is good enough, I think.
class istorage {
 public:
  // Upload.
  virtual void write_file(const std::string& oid, const std::string& raw) = 0;
  // Download.
  virtual std::string read_file(const std::string& oid) = 0;
  virtual bool file_exists(const std::string& oid) = 0;
  virtual size_t get_file_size(const std::string& oid) = 0;

  virtual ~istorage() {
  }
};

};  // namespace storage
