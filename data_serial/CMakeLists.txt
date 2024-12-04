cmake_minimum_required(VERSION 3.28.3)
project(data_serial)

file(GLOB_RECURSE SOURCE
  src/*.cpp
)
add_library(${PROJECT_NAME} ${SOURCE})


target_include_directories(${PROJECT_NAME} PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
  "include"
  "../serial_helpers/include"
  "../parser_serial/include"
  "../embedded_common/attributes/include"
  "../embedded_common/connection/include"
  "../embedded_common/common/include"
  "../embedded_common/data_module_base/include"
  "../embedded_common/parser/include"
)

target_link_libraries(${PROJECT_NAME}
  libmosquitto.so
  serial_helpers
  connections
  common
  data_module_base
  parser_serial
)