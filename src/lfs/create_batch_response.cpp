#include <string>

#include "lfs.h"
#include "server.h"

void create_download_batch_response(const lfs::json_array_t& objects,
                                    const server::data& cfg,
                                    const std::string& authorization,
                                    lfs::batch_response& br) {
  using namespace lfs;

  static const error err{"Object does not exist.",
                         static_cast<int>(object_error_codes::not_found)};

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
          util::get_git_fs_path(cfg.file_directory, oid));
    }

    operation actions = {
        {authorization},
        util::get_url(cfg.scheme, cfg.host, std::to_string(cfg.port),
                      cfg.download_object_path + obj.oid),
    };

    br.objects.push_back({actions, {}, {}, obj});
  }
}

void create_upload_batch_response(const lfs::json_array_t& objects,
                                  const server::data& cfg,
                                  const std::string& authorization,
                                  lfs::batch_response& br) {
  using namespace lfs;

  static const error empty_error {};
  
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

    br.objects.push_back({actions, verify, empty_error, obj});
  }
}

lfs::batch_response lfs::create_batch_response(const std::string& op,
                                               const json_array_t& objects,
                                               const server::data& cfg,
                                               const std::string& authorization) {
  batch_response br {};

  if (op == "download") {
    create_download_batch_response(objects, cfg, authorization, br);
  } else if (op == "upload") {
    create_upload_batch_response(objects, cfg, authorization, br);
  }

  return br;
}
