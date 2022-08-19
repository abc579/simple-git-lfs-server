#pragma once

#include <memory>
#include <string>

#include "storage/istorage.h"

std::unique_ptr<storage::istorage> get_storage_option(const std::string&,
                                                      const std::string&);
