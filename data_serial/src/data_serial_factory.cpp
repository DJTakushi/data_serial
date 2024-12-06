#include "data_serial_factory.h"
#include "data_serial.h"
std::shared_ptr<ec::data_module_i> data_serial_factory::create(nlohmann::json
                                                              config){
  return std::make_shared<data_serial>(config);
}