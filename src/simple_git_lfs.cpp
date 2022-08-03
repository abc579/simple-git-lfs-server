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
    // @FIXME: wrap this up.
    auto j = json11::Json::object {
        { "transfer", br.transfer },
        { "hash_algo", hash_algo }
    };

    /* Traverse objects and append values to the json.�*/
    for (const auto &o : br.objects) {
        auto jtemp = json11::Json::object {
            { "oid", o.object.oid},
            { "size", o.object.size},
            {   "error", json11::Json::object {
                    { "code", o.error.code},
                    { "message", o.error.message}
                }
            }
        };
        // @TEMP: handle arrays.
        j["objects"] = json11::Json::array { jtemp };
    }

    return json11::Json(j).dump();
}

void
sgl::create_batch_response(const json &request, httplib::Response &response)
{
    response.set_header("Content-Type", content_type_lfs);

    batch_response br;

    std::cout << "Dumping...\n";
    std::cout << request.get_value().dump() << std::endl;

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
