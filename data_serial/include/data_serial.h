#pragma once
#include "boost/asio.hpp"
#include "boost/asio/serial_port.hpp"
#include "boost/algorithm/string.hpp"

#include "connection_factory.h"
typedef std::chrono::system_clock::time_point sys_tp;

class data_serial{
  connection_type connection_type_;
  std::shared_ptr<boost::asio::serial_port> serial_port_;
  std::shared_ptr<connection_i> local_conn_;
  bool is_active_{false};
  boost::asio::io_service m_ioService_;
  std::string get_serial_line();

  std::thread work_loop_thread_;
  void work_loop();

 public:
  data_serial(connection_type conn_type);
  ~data_serial();
  void setup();
  void start_work_loop();
  void close();

  bool is_active();
};