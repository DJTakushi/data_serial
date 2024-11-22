#include "data_serial_factory.h"
#include "data_serial.h"
std::shared_ptr<data_module_i> data_serial_factory::create(
    std::string name,
    std::string pub_key,
    connection_type conn_type){
  return std::make_shared<data_serial>(name,pub_key,conn_type);
}
