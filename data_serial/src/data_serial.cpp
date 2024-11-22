#include <iostream>
#include "nlohmann/json.hpp"

#include "environment_helpers.h"
#include "time_helper.h"
#include "data_serial.h"

data_serial::data_serial(std::string name,
                        std::string pub_key,
                        connection_type conn_type) :
    data_module_base(name, pub_key,conn_type){
  std::cout  << time_helper::time_rfc_3339() <<" : ";
  std::cout  << std::string(DATA_SERIAL_VERSION) << " constructing..." <<
      std::endl;
}

data_serial::~data_serial() {
  close();
}

void data_serial::setup(){
  setup_local_conn();

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

nlohmann::ordered_json data_serial::gen_attributes_from_serial(std::string str,
                                                              sys_tp time){
  nlohmann::ordered_json j;
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

    j.emplace_back(attr);
    counter++;
  }

  return j;
}

void data_serial::close(){
  is_active_  =  false;
  stop_all_threads();
  if(serial_port_!= NULL){
    serial_port_->close();
  }
  if(local_conn_ != NULL){
    local_conn_->close();
  }
}

void data_serial::receive_data(){
  std::string str = get_serial_line();
  {
    std::unique_lock lk(receive_data_mutex_);
    lines_read_.push(str);
  }
  receive_data_cv_.notify_one();
}

void data_serial::update_data(){
  std::string str;
  sys_tp time_;
  {
    /* wait for lock message to be avaialble;
    lock mutex while poping message*/
    std::unique_lock lk(receive_data_mutex_);
    receive_data_cv_.wait(lk, [this] { return this->lines_read_.size() > 0; });
    str = lines_read_.front();
    lines_read_.pop();
    time_ = std::chrono::system_clock::now();
  }
  nlohmann::json attr = gen_attributes_from_serial(str,time_);
  attribute_host_.update_attributes_from_array(attr);

  publish_data();
}