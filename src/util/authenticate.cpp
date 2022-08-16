#include "server.h"
#include "util.h"

bool util::authenticate(const user_data& credentials,
                        const server::data& cfg) {
  return credentials.user == cfg.user && credentials.passwd == cfg.passwd;
}
