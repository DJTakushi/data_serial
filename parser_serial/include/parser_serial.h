#pragma once
#include <iostream>
#include <string>
#include "parser_i.h"
#include "attribute_datatype_enum.h"
struct serial_def{
  std::string name_;
  uint pos_;
  ec::Datatype type_;
  serial_def(std::string name, uint pos, ec::Datatype type);
};
class parser_serial : public ec::parser_i {
  std::map<uint,serial_def> def_map_;
  char line_delim_;
  std::string field_delim_;
  void configure_attributes(nlohmann::json config);
 public:
  void configure(nlohmann::json config);
  nlohmann::json get_attributes_from_data(void* data, uint64_t epoch);
  nlohmann::json get_all_supported_attributes();
  nlohmann::json get_config();
  char get_line_delim();
};