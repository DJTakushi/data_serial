#pragma once
#include <memory>
#include "data_module_i.h"

class data_serial_factory{
 public:
  static std::shared_ptr<ec::data_module_i> create(std::string name,
                                                std::string pub_key,
                                                ec::connection_type conn_type,
                                                std::string address,
                                                uint port);
};