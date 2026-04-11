find_package(nlohmann_json REQUIRED)

add_subdirectory(memtable)

add_library(storage
  storage.cpp
)

target_include_directories(storage
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(storage
  PRIVATE
    memtable
    file_store
    nlohmann_json::nlohmann_json
)

target_compile_features(storage
  PUBLIC cxx_std_20
)
