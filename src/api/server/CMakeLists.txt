add_library(server STATIC
  server.cpp
  server.h
)

target_link_libraries(server PRIVATE
  Boost::system
)

target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
