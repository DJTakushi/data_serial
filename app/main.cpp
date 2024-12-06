#include <csignal>
#include <iostream>
#include "arg_helper.h"
#include "data_serial_factory.h"


std::shared_ptr<ec::data_module_i> data_serial_; // global for handlers
void exit_application(int signum) {
  std::cout  << "exiting application..."<<std::endl;
  exit(0);
}

void sig_int_handler(int signum) {
  // TODO : try relocating serial_port_->close() to exit_application()
  if(data_serial_!= NULL){
    data_serial_->exit();
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

  data_serial_ = data_serial_factory::create("data_serial",
                                              "een",
                                              "datsa_serial_config",
                                              type,
                                              address,
                                              port);
  data_serial_->setup();
  data_serial_->start_running();
  while(!(data_serial_->is_exited())){
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  std::cout << "...exiting main function..." << std::endl;
}