#pragma once
#include "nlohmann/json.hpp"
#include "data_module_base_config.h"

struct data_serial_config : public ec::data_module_base_config{
  std::string hardware_name;
  uint port;

  data_serial_config(nlohmann::json j);

  static bool extract_hardware_name(nlohmann::json j, std::string& name);
  static bool extract_hardware_baudrate(nlohmann::json j, uint& baud);
};

