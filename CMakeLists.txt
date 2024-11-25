cmake_minimum_required(VERSION 3.28.3)

set(DATA_SERIAL_PROJECT_NAME        "data_serial")
set(DATA_SERIAL_PROJECT_DESCRIPTION "Data Serial")
set(DATA_SERIAL_AUTHOR_DOMAIN       "https://github.com/DJTakushi/data_serial")
set(DATA_SERIAL_V_MAJOR "0")
set(DATA_SERIAL_V_MINOR "0")
set(DATA_SERIAL_V_PATCH "40")

set(DATA_SERIAL_VERSION
    "${DATA_SERIAL_V_MAJOR}.${DATA_SERIAL_V_MINOR}.${DATA_SERIAL_V_PATCH}")
set(DATA_SERIAL_NAME_VERSION
    "${DATA_SERIAL_PROJECT_NAME} v${DATA_SERIAL_VERSION}")

add_compile_definitions(DATA_SERIAL_VERSION="${DATA_SERIAL_NAME_VERSION}")

add_subdirectory(app)
add_subdirectory(data_serial)
add_subdirectory(embedded_common)
add_subdirectory(serial_helpers)
add_subdirectory(parser_serial)