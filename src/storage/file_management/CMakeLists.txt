add_library(file_management
  file_writing.cpp
  file_reading.cpp
  file_store.cpp
)

target_include_directories(file_management
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(file_management
  PUBLIC cxx_std_20
)
