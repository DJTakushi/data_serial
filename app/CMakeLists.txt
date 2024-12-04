set(TARGET app)

add_executable(${TARGET} main.cpp)

target_compile_features(${TARGET} PUBLIC cxx_std_17)

target_include_directories(${TARGET} PRIVATE
  "../data_serial/include"
  "../serial_helpers/include"
  "../embedded_common/connection/include"
  "../embedded_common/common/include"
  "../embedded_common/data_module_base/include"
)

target_link_libraries(${TARGET}
  libmosquitto.so
  data_serial
  connections
  common
)
