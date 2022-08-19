#include <string>

#include "istorage.h"

namespace storage {

class local_storage : public istorage {
 public:
  explicit local_storage(const std::string &);
  ~local_storage() = default;

 public:
  void write_file(const std::string &, const std::string &) override;
  std::string read_file(const std::string &) override;
  bool file_exists(const std::string &) override;
  size_t get_file_size(const std::string &) override;

 private:
  std::string file_directory_;
};

};  // namespace storage
