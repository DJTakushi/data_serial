#include <boost/algorithm/string.hpp>
#include <chrono>
#include "time_helper.h"
#include "parser_serial.h"
typedef std::chrono::system_clock::time_point sys_tp;

void parser_serial::configure(nlohmann::json config){
  /** TODO: */
}
nlohmann::json parser_serial::get_data(void* data){ // TODO : rename get_attributes_from_data
  nlohmann::json j;
  std::string* str = (std::string*)(data);
  sys_tp time = std::chrono::system_clock::now();
  std::vector<std::string> parts;
  boost::split(parts,*str,boost::is_any_of(","));

  size_t counter = 0;
  auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count(); // TODO : creat common implementation so users don't need chrono
  for(auto part : parts) {
    nlohmann::json attr;
    if(counter < def_map_.size()){
      boost::trim(part);

      serial_def& def = def_map_[counter];

      std::string metric_name = "metric"+std::to_string(counter);
      attr["name"] = def.name_;
      attr["datatype"] = def.type_;
      switch (def.type_){
        case kInteger:
          attr["value"] = std::stoi(part);
          break;
        case kDouble:
          attr["value"] = std::stod(part);
          break;
        case kString:
        default:
          attr["value"] = part;
      }

      attr["timestamp"] = epoch;

      j.emplace_back(attr);
    }
    counter++;
  }
  return j;
}
nlohmann::json parser_serial::get_all_supported_attributes(){
  nlohmann::json j;
  /** TODO: */
  return j;
}
nlohmann::json parser_serial::get_config(){
  nlohmann::json j;
  /** TODO: */
  return j;
}