#pragma once
#include "boost/asio.hpp"
#include "boost/asio/serial_port.hpp"
#include "boost/algorithm/string.hpp"

#include "data_module_base.h"
typedef std::chrono::system_clock::time_point sys_tp;

class data_serial : public ec::data_module_base {
  static bool extract_hardware_name(nlohmann::json j, std::string& name);
  static bool extract_hardware_baudrate(nlohmann::json j, uint& baud);
  char get_line_delim_from_parser();
 protected:
  std::string port_name_;
  uint baud_rate_;
  std::shared_ptr<boost::asio::serial_port> serial_port_;
  boost::asio::io_service m_ioService_;
  std::shared_ptr<std::string> get_serial_line();

  bool is_config_good(nlohmann::json j) override;
  void config_from_json(nlohmann::json j) override;
  void receive_data();
  void update_data();

  void stop();
  nlohmann::ordered_json config_hardware_gen();
 public:
  data_serial(nlohmann::json config);
  ~data_serial();
  void setup();
};