add_subdirectory(routes)
add_subdirectory(helpers)

add_library(ingestion
  server.cpp
)

target_include_directories(ingestion
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(ingestion
  PRIVATE routes
          storage
          helpers
          Boost::system
          nlohmann_json::nlohmann_json
)

target_compile_features(ingestion
  PUBLIC cxx_std_20
)
