#pragma once

#include <memory>
#include <string>

#include "storage/istorage.h"

// This function just returns the corresponding object based on
// `OPTION`.  It acts like a factory method (in OOP) but I did not
// create a class because a function was enough. Or should I?
std::unique_ptr<storage::istorage> get_storage_option(const std::string& option,
                                                      const std::string& file_directory);
