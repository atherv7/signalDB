cmake_minimum_required(VERSION 3.20)
project(signalDB
  VERSION 0.0.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_BUILD_TYPE Debug)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

enable_testing()

find_package(GTest REQUIRED CONFIG)
find_package(Boost REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED CONFIG)

add_subdirectory(src)
add_subdirectory(test)
