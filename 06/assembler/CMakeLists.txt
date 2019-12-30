cmake_minimum_required(VERSION 3.10)
project(assembler)

# C++17 will be used
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin/)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "Build type is: ${CMAKE_BUILD_TYPE}")

# including headers, some generators need it
set(SOURCE_FILES src/main.cpp src/core/assembler.cpp src/core/parser.cpp src/driver.cpp)
set(HEADER_FILES src/core/assembler.hpp src/core/instructions.hpp src/core/parser.hpp src/driver.hpp)

add_executable(assembler ${SOURCE_FILES} ${HEADER_FILES})

target_compile_options(assembler PRIVATE -Wall -Wextra -pedantic)
target_link_libraries(assembler stdc++fs)

