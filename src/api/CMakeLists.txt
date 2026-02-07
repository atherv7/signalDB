add_subdirectory(server)

add_executable(api main.cpp)

target_link_libraries(api PRIVATE
  server
)
