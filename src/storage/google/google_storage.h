#include <string>
#include <vector>

#include "istorage.h"

namespace storage {

class google_storage : public istorage {
 public:
  explicit google_storage(const std::string&);
  ~google_storage() = default;

 public:
  void write_file(const std::string&,
                  const std::string&) override; /* Upload. */
  std::vector<unsigned char> read_file(
      const std::string&) override; /* Download. */
  bool file_exists(const std::string&) override;
  size_t get_file_size(const std::string&) override;

 private:
  std::string file_directory_;
};

};  // namespace storage
