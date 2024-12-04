# data_serial
originally part of spbd repo; see that repor for commit history

```mermaid
---
title: data_serial class diagram
---
classDiagram

namespace embedded_common_repo {

    class connection_azure_routes{
      -IOTHUB_MODULE_CLIENT_LL_HANDLE handle
      +connection_azure_routes(max_messages)
      +bool initialize()
      +start_loop()
      +publish(topic, msg)
      +close();
    }
  class connection_base {
    -message_queue* received_queue_
    -bool active_
    -thread do_work_thread_
    +connection_base(max_messages)
    +stop_loop()
    +bool message_available()
    +string get_received_message()
    +add_message_to_queue(msg)
  }
  class connection_mqtt {
    -bool stable_
    -bool log_mosquitto_
    -list[string] subscriptions_
    -string will_topic_
    -mosquitto* mosq_
    -string host_
    -uint host_port_
    -uint host_keep_alive_
    -bool is_active_
    -service_mqtt()
    -thread mosquitto_service_loop_thread_
    -mosquitto_service_loop()
    -connect_callback(*mosq, *userdata, int result)
    -subscribe_callback(*mosq, *userdata, mid, qos_count, *granted_qos)
    -log_callback(*mosq, *userdata, level, *str)
    -message_callback(*mosq, *userdata, *message)
    +connection_mqtt(host, port, max_messages)
    +bool initialize()
    +start_loop()
    +publish(topic, msg)
    +publish(mid, topic, payloadlen, payload, qos, retain)
    +close()
    +subscriptions_add(sub)
    +set_will_topic(topic)
    +bool is_stable()
  }
  class connection_i{
    +bool initialize() = 0
    +void start_loop() = 0
    +void stop_loop() = 0
    +string get_received_message() = 0
    +bool message_available() = 0
    +publish(topic, msg) = 0
    +close() = 0
    +mutex mutex
    +condition_variable cv
  }
  class data_module_base{
    -connection_type connection_type_
    -bool is_active_
    -thread work_loop_thread_
    -work_loop() = 0
    -setup_local_conn()
    -local_publish(topic, data)
    +data_module_base(conn_type)
    +setup() = 0
    +start_work_loop()
    +void close() = 0
  }
}
connection_base <|-- connection_azure_routes : inherits
connection_base <|-- connection_mqtt : inherits
connection_i <|-- connection_base : inherits
connection_i --* data_module_base : -connection_i* local_conn_

namespace data_serial_repo{
  class data_serial{
    -serial_port* serial_port_
    -io_service m_ioService_
    -string get_serial_line()
    -work_loop()
    +data_serial(conn_type)
    +~data_serial()
    +setup()
    +close()
  }
}

data_module_base  <|-- data_serial : inherits
```
# todo :
- [x] doxygen
  - [x] remove third_party
  - [x] remove python files
- [ ] prove mqtt behavior
  - [ ] mqtt-listener script
- [ ] sample.json update / automation
  - [ ] unit-test a generated json against sample.json
- [x] relocate common files (install_local_depencencies.sh / third_party)
- [ ] reconnection attempt behavior
- [ ] configuration commands
  - [ ] receive channel

# developer notes
- use `socat` for virtual serial
  - https://stackoverflow.com/questions/52187/virtual-serial-port-for-linux
  - https://stackoverflow.com/questions/66789336/instance-of-serialstream-open-dev-ttyusb0-throws-an-exception-bad-file-desc

- https://www.losant.com/blog/how-to-access-serial-devices-in-docker
```
echo "KERNEL==\"ttyUSB[0-9]*\",MODE=\"0666\"" | sudo tee  /etc/udev/rules.d/99-serial.rules
```