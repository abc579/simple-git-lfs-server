#include <string>

#include "server.h"
#include "lfs.h"
#include "logger.h"

lfs::batch_response lfs::create_batch_response(const std::string& op,
                                               const json_array_t& objects,
                                               const server::data& cfg,
                                               const std::string& authorization,
                                               logger::logger& log) {
  static const error err{"Object does not exist.",
                         static_cast<int>(object_error_codes::not_found)};

  batch_response br;

  // @TODO: separate both ifs in different functions.
  if (op == "download") {
    for (const auto& o : objects) {
      const auto oid = o["oid"].string_value();
      const auto size = o["size"].int_value();

      object obj{oid, size};

      // If the name of the object was not provided or the object does not exist
      // in the file system, error.
      if (oid.empty() ||
          !util::can_open(util::get_git_fs_path(cfg.file_directory, oid))) {
        br.objects.push_back({{}, {}, err, obj});
        continue;
      }

      // If the size does not come in the request or they provided zero, obtain
      // it.
      if (size == 0) {
        obj.size = util::get_file_size(
            util::get_git_fs_path(cfg.file_directory, oid), log);
      }

      operation actions = {
          {authorization},
          util::get_url(cfg.scheme, cfg.host, std::to_string(cfg.port),
                        cfg.download_object_path + obj.oid),
      };

      br.objects.push_back({actions, {}, {}, obj});
    }
  } else if (op == "upload") {
    for (const auto& o : objects) {
      object obj{o["oid"].string_value(), o["size"].int_value()};

      operation actions = {
          {authorization},
          util::get_url(cfg.scheme, cfg.host, std::to_string(cfg.port),
                        cfg.upload_object_path + obj.oid),
      };

      verify verify = {
          {authorization},
          util::get_url(cfg.scheme, cfg.host, std::to_string(cfg.port),
                        cfg.verify_object_path),
      };

      br.objects.push_back({actions, verify, {}, obj});
    }
  }

  return br;
}
