add_subdirectory(storage)
add_subdirectory(ingestion)
add_subdirectory(file_management)

add_executable(signalDB
  main.cpp
)

target_link_libraries(signalDB
  PRIVATE storage
          ingestion
          file_management
)
