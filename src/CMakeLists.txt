add_subdirectory(storage)
add_subdirectory(ingestion)

add_executable(signalDB
  main.cpp
)

target_link_libraries(signalDB
  PRIVATE storage
          ingestion
)
