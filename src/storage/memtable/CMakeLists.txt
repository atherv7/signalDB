add_library(memtable
  memtable.cpp
)

target_link_libraries(memtable
  PRIVATE
    nlohmann_json::nlohmann_json
)

target_include_directories(memtable
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(memtable
  PUBLIC cxx_std_20
)
