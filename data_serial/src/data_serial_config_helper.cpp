#include <iostream>

#include "data_serial_config_helper.h"

bool extract_hardware_name(nlohmann::json j, std::string& name){
  bool success = false;
  if(j.contains("hardware")){
    if(j["hardware"].is_object()){
      nlohmann::json hardware_conf = j["hardware"];
      if(hardware_conf.contains("name")){
        if(hardware_conf["name"].is_string()){
          name = hardware_conf["name"];
          success = true;
        }
        else{
          std::cerr << "config.hardware.name is not a string" << std::endl;
        }
      }
      else{
        std::cerr << "config.hardware does not contain name" << std::endl;
      }
    }
    else{
      std::cerr << "config.hardware is not an object" << std::endl;
    }
  }
  else{
    std::cerr << "config.hardware is missing" << std::endl;
  }
  return success;
}
bool extract_hardware_baudrate(nlohmann::json j, uint& baud){
  bool success = false;
  if(j.contains("hardware")){
    if(j["hardware"].is_object()){
      nlohmann::json hardware_conf = j["hardware"];
      if(hardware_conf.contains("baud")){
        if(hardware_conf["baud"].is_number_integer()){
          baud = hardware_conf["baud"];
          success = true;
        }
        else{
          std::cerr << "config.hardware.name is not a string" << std::endl;
        }
      }
      else{
        std::cerr << "config.hardware does not contain name" << std::endl;
      }
    }
    else{
      std::cerr << "config.hardware is not an object" << std::endl;
    }
  }
  else{
    std::cerr << "config.hardware is missing" << std::endl;
  }
  return success;
}