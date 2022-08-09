#include <cstdlib>

#include "config.h"
#include "httplib.h"
#include "simple_git_lfs.h"
#include "server.h"

int
main()
{
    std::ios::sync_with_stdio(false);

    httplib::Server server;
    
    server.Post("/objects/batch", sgls::batch_request_handler);
    server.Get(R"(/objects/[a-zA-Z0-9]*$)", sgls::download_handler);
    server.Put(R"(/objects/[a-zA-Z0-9]*$)", sgls::upload_handler);
    
    server.listen(sgls::sv_host, sgls::sv_port);

    return EXIT_SUCCESS;
}
