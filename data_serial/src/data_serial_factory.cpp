#include "data_serial_factory.h"
#include "data_serial.h"
std::shared_ptr<ec::data_module_i> data_serial_factory::create(
    std::string name,
    std::string pub_key,
    ec::connection_type conn_type,
    std::string address,
    uint port){
  return std::make_shared<data_serial>(name,pub_key,conn_type,address,port);
}
