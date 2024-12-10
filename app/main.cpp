#include <csignal>
#include <fstream>
#include <iostream>
#include "arg_helper.h"
#include "data_serial_factory.h"
#include "data_module_base_config.h"


std::shared_ptr<ec::data_module_i> data_serial_; // global for handlers
void exit_application(int signum) {
  std::cout  << "exiting application..."<<std::endl;
  exit(0);
}

void sig_int_handler(int signum) {
  // TODO : try relocating serial_port_->close() to exit_application()
  if(data_serial_!= NULL){
    data_serial_->command_exit();
  }
  std::cout << "ctrl+c pressed, exiting..."<<std::endl;
  exit_application(1);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, sig_int_handler); // registar for ctrl-c
  signal(SIGTERM, exit_application); // terminate from Docker STOPSIGNAL
  ec::connection_type type = ec::arg_helper::get_connection_type(argc,argv);
  std::string address = ec::arg_helper::get_address(argc,argv);
  uint port = ec::arg_helper::get_port(argc,argv);

  std::ifstream ifs("data_serial_config.json");
  nlohmann::json config = nlohmann::json::parse(ifs);
  ec::data_module_base_config::set_local_conn_type(config, type);
  ec::data_module_base_config::set_local_conn_address(config, address);
  ec::data_module_base_config::set_local_conn_port(config, port);

  data_serial_ =  data_serial_factory::create(config);
  data_serial_->command_run();
  while(!(data_serial_->is_exited())){
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  std::cout << "...exiting main function..." << std::endl;
}