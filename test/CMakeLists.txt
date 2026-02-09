find_package(GTest REQUIRED CONFIG)
find_package(Boost REQUIRED CONFIG)

add_executable(storage_tests
  storage_tests.cpp
  helpers.cpp
)
target_link_libraries(storage_tests
  PRIVATE
    storage
    GTest::gtest
    GTest::gtest_main
    Boost::system
    nlohmann_json::nlohmann_json
)
add_test(NAME StorageTests COMMAND storage_tests)

add_executable(api_tests
  api_tests.cpp
  helpers.cpp
)
target_link_libraries(api_tests
  PRIVATE
    storage
    api
    GTest::gtest
    GTest::gtest_main
    Boost::system
    nlohmann_json::nlohmann_json
)
add_test(NAME APITests COMMAND api_tests)
