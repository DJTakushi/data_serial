#include <iostream>
#include "data_serial.h"


std::shared_ptr<data_serial> data_serial_; // global for handlers
void exit_application(int signum) {
  std::cout  << "exiting application..."<<std::endl;
  exit(0);
}

void sig_int_handler(int signum) {
  // TODO : try relocating serial_port_->close() to exit_application()
  if(data_serial_!= NULL){
    data_serial_->close();
  }
  std::cout << "ctrl+c pressed, exiting..."<<std::endl;
  exit_application(1);
}


int main(int argc, char* argv[]) {
  #ifdef AZURE_ROUTES
    connection_type type = kAzureIot;
  #else
    connection_type type = kMqtt;
  #endif

  data_serial_ = std::make_shared<data_serial>(type);
  data_serial_->setup();
  data_serial_->start_work_loop();
  while(data_serial_->is_active()){
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  data_serial_->close();
  std::cout << "...exiting main function..." << std::endl;
}