add_subdirectory(routes)
add_subdirectory(helpers)

add_library(api
  server.cpp
)

target_include_directories(api
  PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(api
  PRIVATE routes
          storage
          helpers
          Boost::system
          nlohmann_json::nlohmann_json
)

target_compile_features(api
  PUBLIC cxx_std_20
)
