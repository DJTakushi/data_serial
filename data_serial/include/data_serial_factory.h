#pragma once
#include <memory>
 #include <nlohmann/json.hpp>
#include "data_module_i.h"

class data_serial_factory{
 public:
  static std::shared_ptr<ec::data_module_i> create(nlohmann::json config);
};