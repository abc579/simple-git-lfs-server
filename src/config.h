#pragma once

#include <string>

namespace sgls {

     // These values will be overriden by setup_config() if we decide to use
     // ENV variables.
     const std::string sv_host {"0.0.0.0"};
     const std::string sv_user {"temp"};
     const std::string sv_passwd {"temp"};
     const int sv_port {8080};
     
} // namespace sgls
