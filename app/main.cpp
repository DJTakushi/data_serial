#include <csignal>
#include <iostream>
#include "data_serial_factory.h"


std::shared_ptr<data_module_i> data_serial_; // global for handlers
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

connection_type get_connection_type(int argc, char* argv[]){
  connection_type type = kMqtt;
  for(size_t i = 1; i < argc; i++) {
    std::string str(argv[i]);
    if (str.substr(0,2)=="-c"){
      if(str.substr(2,str.npos)==AZURE_ROUTES){
        type = kAzureIot;
        std::cout << "connecion type '"<<AZURE_ROUTES<<"' detected"<<std::endl;
      }
      if(str.substr(2,str.npos)==MQTT){
        type = kMqtt;
        std::cout << "connecion type '"<<MQTT<<"' detected"<<std::endl;
      }
    }
  }
  return type;
};
std::string get_address(int argc, char* argv[]){
  std::string address = "localhost";
  for(size_t i = 1; i < argc; i++){
      std::string str(argv[i]);
    if (str.substr(0,2)=="-a"){
      address = str.substr(2,str.npos);
    }
  }
  return address;
}
uint get_port(int argc, char* argv[]){
  uint port = 1883;
  for(size_t i = 1; i < argc; i++){
      std::string str(argv[i]);
    if (str.substr(0,2)=="-p"){
      port = std::stoi(str.substr(2,str.npos));
    }
  }
  return port;
}

int main(int argc, char* argv[]) {
  signal(SIGINT, sig_int_handler); // registar for ctrl-c
  signal(SIGTERM, exit_application); // terminate from Docker STOPSIGNAL
  connection_type type = get_connection_type(argc,argv);
  std::string address = get_address(argc,argv);
  uint port = get_port(argc,argv);

  data_serial_ = data_serial_factory::create("data_serial",
                                              "data_serial_output",
                                              type,
                                              address,
                                              port);
  data_serial_->setup();
  data_serial_->start_all_threads();
  while(data_serial_->is_active()){
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  data_serial_->close();
  std::cout << "...exiting main function..." << std::endl;
}