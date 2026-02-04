add_executable(storage_tests
  storage_tests.cpp
  helpers.cpp
)

target_link_libraries(storage_tests
  PRIVATE
    storage
    gtest
    gtest_main
)

add_test(NAME StorageTests COMMAND storage_tests)
