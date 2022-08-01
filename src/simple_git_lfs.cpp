#include "simple_git_lfs.h"

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

void
sgl::create_batch_response(const json &request, httplib::Response &response)
{
    response.set_header("Content-Type", content_type_lfs);

    auto objects = request.get_array_items("objects");

    for (const auto &object : objects) {
        std::cout << "oid: " << object["oid"].string_value() << std::endl;
        std::cout << "size: " << object["size"].int_value() << std::endl;
    }
}
