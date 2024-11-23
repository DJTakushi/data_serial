#pragma once
#include "parser_i.h"

class parser_serial : public parser_i {
  void configure(nlohmann::json config);
  nlohmann::json get_data(void*);
  nlohmann::json get_all_supported_attributes();
  nlohmann::json get_config();
};