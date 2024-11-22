#pragma once
#include "boost/asio.hpp"
#include "boost/asio/serial_port.hpp"
#include "boost/algorithm/string.hpp"

#include "data_module_base.h"
typedef std::chrono::system_clock::time_point sys_tp;

class data_serial : public data_module_base {
  std::shared_ptr<boost::asio::serial_port> serial_port_;
  boost::asio::io_service m_ioService_;
  std::string get_serial_line();

  std::queue<std::string> lines_read_;

  void receive_data();
  void update_data();


 public:
  data_serial(std::string pub_key, connection_type conn_type);
  ~data_serial();
  void setup();
  void close();
};