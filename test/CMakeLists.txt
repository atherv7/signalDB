add_executable(storage_tests
  storage_tests.cpp
)

target_link_libraries(storage_tests
  storage
  gtest
  gtest_main
)

add_test(NAME StorageTests COMMAND storage_tests)
