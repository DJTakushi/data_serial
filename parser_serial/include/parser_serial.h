#pragma once
#include "parser_i.h"
#include "attribute_datatype_enum.h"
struct serial_def{
  std::string name_;
  uint pos_;
  Datatype type_;
};
class parser_serial : public parser_i {
  std::map<uint,serial_def> def_map_;
  void configure(nlohmann::json config);
  nlohmann::json get_attributes_from_data(void* data, uint64_t epoch);
  nlohmann::json get_all_supported_attributes();
  nlohmann::json get_config();
};