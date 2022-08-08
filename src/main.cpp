#include "httplib.h"
#include "simple_git_lfs.h"
#include "server.h"
#include <cstdlib>

int
main()
{
    std::ios::sync_with_stdio(false);
    
    const std::string host {"0.0.0.0"};
    const int portno {8080};

    httplib::Server server;

    server.Post("/objects/batch", sgls::batch_request_handler);
    server.Get(R"(/objects/[a-zA-Z0-9]*$)", sgls::download_handler);
    server.listen(host, portno);

    return EXIT_SUCCESS;
}
