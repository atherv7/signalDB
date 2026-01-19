add_library(memtable
  memtable.cpp
)

target_include_directories(memtable
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(memtable
  PUBLIC cxx_std_20
)
