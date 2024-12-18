#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"

#include "environment_helpers.h"
#include "time_helper.h"
#include "parser_serial.h"
#include "data_serial.h"
#include "data_serial_config.h"

data_serial::data_serial(nlohmann::json config) : data_module_base(config){
  std::cout  << ec::time_helper::time_rfc_3339() <<" : ";
  std::cout  << std::string(DATA_SERIAL_VERSION) << " constructing..." << std::endl;
  parser_ = std::make_shared<parser_serial>();

  start_state_machine_loop();
  if(!is_config_good(config)){
    state_ = ec:: kConfigBad; // flag bad config so we can exit
  }
}

data_serial::~data_serial() {
  exit();
}

bool data_serial::is_config_good(nlohmann::json j){
  /** TODO: verify data_serial components are good */
  data_serial_config cfg = data_serial_config(j);
  return cfg.good;
}

void data_serial::config_from_json(nlohmann::json j){
  data_serial_config cfg = data_serial_config(j);
  if(cfg.good){
    data_module_base::config_from_json(j);
    data_serial_config::extract_hardware_name(j,port_name_);
    data_serial_config::extract_hardware_baudrate(j, baud_rate_);
    parser_->configure(j["parser"]);
  }
  else{
    std::cerr << "config not good!" << std::endl;
    state_ = ec::kConfigBad;
  }
}
char data_serial::get_line_delim_from_parser(){
  std::shared_ptr<parser_serial> p = std::static_pointer_cast<parser_serial>(parser_);
  return p->get_line_delim();
}

void data_serial::start(){
  serial_port_ = get_serial_port(m_ioService_, port_name_, baud_rate_);
  if(serial_port_->is_open()){
    setup_local_conn();
    start_all_threads();
    state_ = ec::kRunning;
  }
  else{
    std::cout << "start() failed; exiting..."<<std::endl;
    state_ = ec::kExiting;
  }
}

std::shared_ptr<std::string> data_serial::get_serial_line(){
  /** BLOCKING function to get an '\n' terminating line from serial */
  std::shared_ptr<std::string> line = std::make_shared<std::string>("");
  char c = '0';

  while (c != get_line_delim_from_parser() && is_running()) {
    /** TODO: make asynchronous so this doesn't block */
    serial_port_->read_some(boost::asio::buffer(&c, 1));
    *line += c;
  }
  return line;
}

void data_serial::receive_data(){
  std::shared_ptr<std::string> str = get_serial_line();
  std::shared_ptr<void*> v = reinterpret_cast<std::shared_ptr<void*>&>(str);
  {
    std::unique_lock lk(receive_data_mutex_);
    received_data_.push(v);
  }
  receive_data_cv_.notify_one();
}

void data_serial::update_data(){
  std::shared_ptr<void*> v;
  uint64_t epoch;
  {
    /* wait for lock message to be avaialble;
    lock mutex while poping message*/
    std::unique_lock lk(receive_data_mutex_);
    receive_data_cv_.wait(lk, [this] { return this->received_data_.size() > 0 ||
                                              !is_running(); });
    if(this->received_data_.size() > 0){
      v = received_data_.front();
      received_data_.pop();
    }
  }

  if(v.get() != nullptr){
    epoch = ec::time_helper::get_epoch_now();
    nlohmann::json attr = parser_->get_attributes_from_data((void*)(v.get()),
                                                            epoch);
    attribute_host_.update_attributes_from_array(attr);

    publish_data();
  }
}

void data_serial::stop(){
  data_module_base::stop();
  if(serial_port_!= NULL){
    serial_port_->close();
  }
}
nlohmann::ordered_json data_serial::config_hardware_gen(){
  nlohmann::ordered_json hw_config;
  hw_config["type"] = "serial";
  hw_config["name"] = port_name_;
  hw_config["baud"] = baud_rate_;
  return hw_config;
}
