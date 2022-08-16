#include <iterator>
#include <string>
#include <vector>

#include "lfs.h"

// In this case, the oid comes in the URL, so we have to extract that and
// check if the file exists in the directory. If it does, store the contents
// in a string, put that in RESPONSE and return it.
void lfs::download_handler(const request_t& request, response_t& response,
                           const server::data& cfg) {
  const std::string oid = util::get_oid_from_url(request.path);

  std::ifstream file(util::get_git_fs_path(cfg.file_directory, oid),
                     std::ios::in | std::ios::binary);
  std::noskipws(file);
  std::vector<unsigned char> binary_file(std::istream_iterator<char>(file),
                                         std::istream_iterator<char>{});
  file.close();

  response.set_header("Accept", ACCEPT_LFS);
  response.set_content(std::string(binary_file.begin(), binary_file.end()),
                       "application/octet-stream");
  response.status = static_cast<int>(http_response_codes::ok);
}
