add_library(server STATIC
  server.cpp
  server.h
)

target_link_libraries(server PRIVATE
  Boost::system
  nlohmann_json::nlohmann_json
)

target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
