set(TARGET data_serial)

if (${AZURE_ROUTES})
  add_compile_definitions(AZURE_ROUTES=${AZURE_ROUTES})
endif()

add_executable(${TARGET} main.cpp)

target_compile_features(${TARGET} PUBLIC cxx_std_17)

target_include_directories(${TARGET} PRIVATE
  "../serial_helpers/include"
  "../embedded_common/connection/include"
  "../embedded_common/common/include"
)

target_link_libraries(${TARGET}
  libmosquitto.so
  serial_helpers
  connections
  common
)