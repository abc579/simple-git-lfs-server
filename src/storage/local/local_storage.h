#include <string>
#include <vector>

#include "storage.h"

namespace storage {

class local_storage : public storage::storage {
 public:
  explicit local_storage(const std::string &);
  ~local_storage() = default;

 public:
  void write_file(const std::string &, const std::string &) override;
  std::vector<unsigned char> read_file(const std::string &) override;
  bool file_exists(const std::string &) override;
  size_t get_file_size(const std::string &) override;

 private:
  std::string file_directory_;
};

};  // namespace storage
