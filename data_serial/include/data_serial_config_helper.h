#pragma once
#include "nlohmann/json.hpp"

bool extract_hardware_name(nlohmann::json j, std::string& name);
bool extract_hardware_baudrate(nlohmann::json j, uint& baud);
