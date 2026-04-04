find_package(nlohmann_json CONFIG REQUIRED)

add_library(file_management
  file_store.cpp
)

target_link_libraries(file_management
  PRIVATE
    nlohmann_json::nlohmann_json
)

target_include_directories(file_management
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_features(file_management
  PUBLIC cxx_std_20
)
