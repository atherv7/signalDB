cmake_minimum_required(VERSION 3.14)
project(signalDB LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

add_library(storage
  src/storage.cpp
)
target_include_directories(storage PUBLIC src)

add_executable(app src/main.cpp)
target_link_libraries(app storage)

add_subdirectory(test)
