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

  start_state_machine_loop();
  if(!is_config_good(config)){
    state_ = ec:: kConfigBad; // flag bad config so we can exit
  }
}

data_serial::~data_serial() {
  exit();
}

bool data_serial::extract_hardware_name(nlohmann::json j, std::string& name){
  bool success = false;
  if(j.contains("hardware")){
    if(j["hardware"].is_object()){
      nlohmann::json hardware_conf = j["hardware"];
      if(hardware_conf.contains("name")){
        if(hardware_conf["name"].is_string()){
          name = hardware_conf["name"];
          success = true;
        }
        else{
          std::cerr << "config.hardware.name is not a string" << std::endl;
        }
      }
      else{
        std::cerr << "config.hardware does not contain name" << std::endl;
      }
    }
    else{
      std::cerr << "config.hardware is not an object" << std::endl;
    }
  }
  else{
    std::cerr << "config.hardware is missing" << std::endl;
  }
  return success;
}
bool data_serial::extract_hardware_baudrate(nlohmann::json j, uint& baud){
  bool success = false;
  if(j.contains("hardware")){
    if(j["hardware"].is_object()){
      nlohmann::json hardware_conf = j["hardware"];
      if(hardware_conf.contains("baud")){
        if(hardware_conf["baud"].is_number_integer()){
          baud = hardware_conf["baud"];
          success = true;
        }
        else{
          std::cerr << "config.hardware.name is not a string" << std::endl;
        }
      }
      else{
        std::cerr << "config.hardware does not contain name" << std::endl;
      }
    }
    else{
      std::cerr << "config.hardware is not an object" << std::endl;
    }
  }
  else{
    std::cerr << "config.hardware is missing" << std::endl;
  }
  return success;
}

bool data_serial::is_config_good(nlohmann::json j){
  bool is_good = data_module_base::is_config_good(j);
  /** TODO: verify data_serial components are good */
  std::string tmp_hardware_name;
  is_good &= extract_hardware_name(j,tmp_hardware_name);

  uint tmp_baudrate;
  is_good &= extract_hardware_baudrate(j, tmp_baudrate);
  return is_good;
}

void data_serial::config_from_json(nlohmann::json j){
  data_module_base::config_from_json(j);

  extract_hardware_name(j,port_name_);
  extract_hardware_baudrate(j, baud_rate_);

  /** TODO: make parser-generation an overloaded function and call from base class */
  nlohmann::json attr_config = j["parser"]["attributes"];
  parser_ = std::make_shared<parser_serial>();
  parser_->configure(attr_config);

  nlohmann::json parser_attributes = parser_->get_all_supported_attributes();
  attribute_host_.update_attributes_from_array(parser_attributes);
}

void data_serial::setup(){
  /** TODO: close & set up ONLY the things that need to be set up */
  setup_local_conn();

  serial_port_ = get_serial_port(m_ioService_, port_name_, baud_rate_);
  start_all_threads();
  state_ = ec::kRunning;
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
