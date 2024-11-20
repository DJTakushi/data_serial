cmake_minimum_required(VERSION 3.28.3)
project(data_serial)

if (${AZURE_ROUTES})
  add_compile_definitions(AZURE_ROUTES=${AZURE_ROUTES})
endif()

file(GLOB_RECURSE SOURCE
  src/*.cpp
)
add_library(${PROJECT_NAME} ${SOURCE})


target_include_directories(${PROJECT_NAME} PRIVATE
  "${PROJECT_SOURCE_DIR}/include"
  "include"
  "../serial_helpers/include"
  "../embedded_common/connection/include"
  "../embedded_common/common/include"
  "../embedded_common/data_module_base/include"
)

target_link_libraries(${PROJECT_NAME}
  libmosquitto.so
  serial_helpers
  connections
  common
  data_module_base
)