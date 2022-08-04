#include "simple_git_lfs.h"
#include <string>

void
sgl::batch_handler(const httplib::Request &request, httplib::Response &response)
{
    /* Parse request and create response. */
    try {
        auto json_request = json(request.body);
        create_batch_response(json_request, response);
    } catch (const json_parse_error &jpe) {
        std::cerr << jpe.what() << std::endl;
    }
}

std::string
sgl::encode_batch_response(const batch_response &br)
{
    // @FIXME: wrap this up and refactor.
    auto j = json11::Json::object {
        { "transfer", br.transfer },
        { "hash_algo", hash_algo }
    };

    auto object_array = json11::Json::array();

    /* Traverse objects and append values to the json.�*/
    for (const auto &o : br.objects) {

        json11::Json::object jtemp;
        const std::string download_link { "http://localhost:8080/objects/" + o.object.oid };

        if (can_open(file_directory + o.object.oid)) {
            jtemp = {
                { "oid", o.object.oid },
                { "size", o.object.size},
                {   "actions", json11::Json::object {
                        {   "download", json11::Json::object {
                                { "href", download_link},
                                { "expires_in", 3600} // @FIXME
                            }
                        }
                    }
                }
            };
        } else {
            jtemp = {
                { "oid", o.object.oid},
                { "size", o.object.size},
                {   "error", json11::Json::object {
                        { "code", o.error.code},
                        { "message", o.error.message}
                    }
                }
            };
        }

        object_array.push_back(jtemp);
    }

    j["objects"] = object_array;

    return json11::Json(j).dump();
}

void
sgl::create_batch_response(const json &request, httplib::Response &response)
{
    response.set_header("Content-Type", content_type_lfs);
    response.set_header("Accept", accept_lfs);

    batch_response br;

    auto objects = request.get_array_items("objects");
    for (const auto &object : objects) {
        // @TODO: find objects in path.
        // @TODO: Do not rely on string_value() and int_value(), wrap them up!
        error_object error { 404, "Object does not exist."};
        batch_object bo { object["oid"].string_value(), object["size"].int_value() };
        br.objects.push_back({bo, {}, error});
    }

    response.set_content(encode_batch_response(br), "application/json");
}

bool
sgl::can_open(const std::string &path)
{
    std::ifstream ifs ( path );

    return ifs.is_open();
}

void
sgl::download_handler(const httplib::Request &request, httplib::Response &response)
{
    // @FIXME all of this needs to be refactored.
    // @FIXME: when the file is too large, git lfs client does it in chunks using range,
    // so we have to do adapt to that situation.
    const std::string raw = []() {
        std::ifstream file {
            file_directory + "541b3e9daa09b20bf85fa273e5cbd3e80185aa4ec298e765db87742b70138a53",
            std::ios::binary };

        if (!file) {
            std::cerr << "File could not be opened." << std::endl;
        }

        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }
    ();

    response.set_header("Content-Type", "application/octet-stream");
    response.set_header("Content-Length", "1000");
    response.set_content(raw, "text/plain");
}
