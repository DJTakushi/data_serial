#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"

#include "environment_helpers.h"
#include "time_helper.h"
#include "parser_serial.h"
#include "data_serial.h"

data_serial::data_serial(nlohmann::json config) : data_module_base(config){
  std::cout  << ec::time_helper::time_rfc_3339() <<" : ";
  std::cout  << std::string(DATA_SERIAL_VERSION) << " constructing..." << std::endl;

  config_from_json(config);
}

data_serial::~data_serial() {
  exit();
}

void data_serial::config_from_json(nlohmann::json j){
  std::cout << "data_serial::config_from_json" << std::endl;
  state_ = ec::data_module_status::kConfiguring;
  data_module_base::config_from_json(j);
  nlohmann::json attr_config = j["parser"]["attributes"];
  parser_ = std::make_shared<parser_serial>();
  parser_->configure(attr_config);
  nlohmann::json parser_attributes = parser_->get_all_supported_attributes();
  attribute_host_.update_attributes_from_array(parser_attributes);

  setup();
}

void data_serial::setup(){
  /** TODO: close & set up ONLY the things that need to be set up */
  setup_local_conn();

  serial_port_ = get_serial_port(m_ioService_);
}

std::shared_ptr<std::string> data_serial::get_serial_line(){
  /** BLOCKING function to get an '\n' terminating line from serial */
  std::shared_ptr<std::string> line = std::make_shared<std::string>("");
  char c = '0';

  while (c != '\n' && is_running()) {
    /** TODO: make asynchronous so this doesn't block */
    serial_port_->read_some(boost::asio::buffer(&c, 1));
    *line += c;
  }
  return line;
}

void data_serial::exit(){
  state_ = ec::data_module_status::kExiting;
  stop_all_threads();
  if(serial_port_!= NULL){
    serial_port_->close();
  }
  if(local_conn_ != NULL){
    local_conn_->close();
  }
  state_ = ec::data_module_status::kExited;
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