#include "httplib.h"
#include "simple_git_lfs.h"

int
main()
{
    httplib::Server s;

    s.Post("/objects/batch", sgl::batch_handler);

    s.listen("0.0.0.0", 8080);

    return EXIT_SUCCESS;
}
