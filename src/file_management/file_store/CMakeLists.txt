find_package(nlohmann_json CONFIG REQUIRED)

add_library(file_store
  file_store.cpp
)

target_link_libraries(file_store
  PRIVATE
    nlohmann_json::nlohmann_json
)

target_include_directories(file_store
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(file_store
  PUBLIC cxx_std_20
)
