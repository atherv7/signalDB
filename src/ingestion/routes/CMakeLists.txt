add_library(routes STATIC
  routes.cpp
  routes.h
)

target_link_libraries(routes PRIVATE
  Boost::system
  nlohmann_json::nlohmann_json
  storage
)

target_include_directories(routes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
