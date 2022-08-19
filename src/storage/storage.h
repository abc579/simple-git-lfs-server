#pragma once

#include <string>
#include <vector>

namespace storage {

class storage {
 public:
  virtual void write_file(const std::string& oid,
                          const std::string& raw) = 0; /* Upload. */
  virtual std::vector<unsigned char> read_file(
      const std::string& oid) = 0; /* Download. */
  virtual bool file_exists(const std::string& oid) = 0;
  virtual size_t get_file_size(const std::string& oid) = 0;

  virtual ~storage() {}
};

};  // namespace storage
