add_subdirectory(storage)
add_subdirectory(api)

add_executable(signalDB
  main.cpp
)

target_link_libraries(signalDB
  PRIVATE storage
          api
)
