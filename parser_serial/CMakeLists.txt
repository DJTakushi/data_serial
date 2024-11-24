cmake_minimum_required(VERSION 3.28.3)

project(parser_serial)

file(GLOB_RECURSE SOURCE
  ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE
  "include"
  "../embedded_common/common/include"
  "../embedded_common/parser/include"
)

target_link_libraries(${PROJECT_NAME}
  common
)