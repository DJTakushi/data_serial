#include <boost/algorithm/string.hpp>
#include "time_helper.h"
#include "parser_serial.h"

serial_def::serial_def(std::string name, uint pos, ec::Datatype type) :
    name_(name),
    pos_(pos),
    type_(type) {
}

void parser_serial::configure(nlohmann::json config){
  if(config.is_array()){
    for(auto attr = config.begin(); attr != config.end(); ++attr){
      if(attr->contains("name") &&
          attr->contains("datatype") && 
          attr->contains("position")){
        if((*attr)["name"].is_string() &&
            (*attr)["datatype"].is_number_integer() &&
            (*attr)["position"].is_number_integer()){
          std::string name = attr->at("name");
          uint pos = attr->at("position");
          ec::Datatype datatype = attr->at("datatype");
          def_map_.emplace(pos,serial_def(name,pos,datatype));
        }
      }
    }
  }
}
nlohmann::json parser_serial::get_attributes_from_data(void* data,
                                                      uint64_t epoch){
  nlohmann::json j;
  std::string* str = (std::string*)(data);
  boost::trim(*str);

  std::vector<std::string> parts;
  boost::split(parts,*str,boost::is_any_of(","));

  size_t counter = 0;
  for(auto part : parts) {
    nlohmann::json attr;
    if(counter < def_map_.size()){
      boost::trim(part);

      serial_def def = def_map_.at(counter+1);

      attr["name"] = def.name_;
      attr["datatype"] = def.type_;

      switch (def.type_){
        case ec::kInteger:
          attr["value"] = std::stoi(part);
          break;
        case ec::kDouble:
          try{
            attr["value"] = std::stod(part);
          }
          catch (std::invalid_argument){
            std::cout << "stod : std::invalid_argument exception in str : ";
            std::cout << *str <<std::endl;
          }
          break;
        case ec::kString:
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
  for(auto ds : def_map_){
    nlohmann::json def;
    def["name"] = ds.second.name_;
    def["datatype"] = ds.second.type_;
    j.emplace_back(def);
  }
  return j;
}
nlohmann::json parser_serial::get_config(){
  nlohmann::json j;
  for(auto ds : def_map_){
    nlohmann::json def;
    def["name"] = ds.second.name_;
    def["datatype"] = ds.second.type_;
    def["position"] = ds.second.pos_;
    j.emplace_back(def);
  }
  return j;
}