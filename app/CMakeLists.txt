set(TARGET data_serial)
target_compile_features(${TARGET} PUBLIC cxx_std_17)

if (${AZURE_ROUTES})
  add_compile_definitions(AZURE_ROUTES=${AZURE_ROUTES})
endif()

file(GLOB_RECURSE SOURCE
  src/*.cpp
)

add_library(${TARGET} ${SOURCE} )

target_compile_features(${TARGET} PUBLIC cxx_std_17)

target_include_directories(${TARGET} PRIVATE
  "../data_serial/include"
  "../serial_helpers/include"
  "../embedded_common/connection/include"
  "../embedded_common/common/include"
)

target_link_libraries(${TARGET}
  libmosquitto.so
  data_serial
  connections
  common
)