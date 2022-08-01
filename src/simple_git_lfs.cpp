#include "simple_git_lfs.h"

void
sgl::batch_handler(const httplib::Request &request, httplib::Response &response)
{
    auto json_request = json11::Json(request.body);

    std::cout << json_request.dump() << std::endl;
    std::cout << "operation:" << json_request["operation"].string_value() << std::endl;

    response.set_content("Let's all love Lain\r\n", "text/plain");
}
