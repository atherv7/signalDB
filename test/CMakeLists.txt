find_package(GTest REQUIRED CONFIG)

add_executable(storage_tests
  storage_tests.cpp
  helpers.cpp
)

target_link_libraries(storage_tests
  PRIVATE
    storage
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME StorageTests COMMAND storage_tests)
