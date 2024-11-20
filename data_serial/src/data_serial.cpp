#include <iostream>
#include "nlohmann/json.hpp"

#include "environment_helpers.h"
#include "time_helper.h"
#include "data_serial.h"

data_serial::data_serial(connection_type conn_type) :
    data_module_base(conn_type){
  std::cout  << time_helper::time_rfc_3339() <<" : ";
  std::cout  << std::string(DATA_SERIAL_VERSION) << " constructing..." <<
      std::endl;
}

data_serial::~data_serial() {
  close();
}

void data_serial::setup(){
  local_conn_ = connection_factory::create(connection_type_);

  serial_port_ = get_serial_port(m_ioService_);
}

std::string data_serial::get_serial_line(){
  /** BLOCKING function to get an '\n' terminating line from serial */
  std::string line = "";
  char c = '0';

  while (c != '\n') {
    serial_port_->read_some(boost::asio::buffer(&c, 1));
    line += c;
  }
  return line;
}

nlohmann::ordered_json gen_metrics_from_serial(std::string str, sys_tp time){
  nlohmann::ordered_json j;
  j["name"]="data_serial";
  std::vector<std::string> parts;
  boost::split(parts,str,boost::is_any_of(","));

  size_t counter = 0;
  auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
  for(auto part : parts) {
    nlohmann::json attr;
    std::string metric_name = "metric"+std::to_string(counter);
    boost::trim(part);
    attr["name"] = metric_name;
    attr["datatype"] = 10;
    attr["value"] = std::stod(part);
    attr["timestamp"] = epoch;

    j["attributes"].emplace_back(attr);
    counter++;
  }

  return j;
}

void data_serial::work_loop(){
  while(is_active_){
    std::string str = get_serial_line();
    sys_tp time_(std::chrono::system_clock::now());

    nlohmann::ordered_json j = gen_metrics_from_serial(str, time_);
    // std::cout << j.dump() << std::endl;

    local_publish("data_serial_output",j.dump());
  }
}

void data_serial::close(){
  is_active_  =  false;
  if (work_loop_thread_.joinable()){
    work_loop_thread_.join();
  }
  if(serial_port_!= NULL){
    serial_port_->close();
  }
  if(local_conn_ != NULL){
    local_conn_->close();
  }
}
