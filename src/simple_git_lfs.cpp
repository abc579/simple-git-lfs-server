#include "simple_git_lfs.h"

void
sgl::batch_handler(const httplib::Request &request, httplib::Response &response)
{
    try {
        auto json_request = json(request.body);
        std::cout << json_request.get_string_value("operation") << std::endl;
    } catch (const json_parse_error &jpe) {
        std::cerr << jpe.what() << std::endl;
    }

    response.set_content("Let's all love Lain\r\n", "text/plain");
}
