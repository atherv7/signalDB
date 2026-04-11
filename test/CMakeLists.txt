find_package(GTest REQUIRED CONFIG)
find_package(Boost REQUIRED CONFIG)

add_executable(storage_tests
  storage_tests.cpp
  helpers.cpp
)
target_link_libraries(storage_tests
  PRIVATE
    storage
    file_management
    GTest::gtest
    GTest::gtest_main
    Boost::system
    nlohmann_json::nlohmann_json
)
add_test(NAME StorageTests COMMAND storage_tests)

add_executable(ingestion_tests
  ingestion_tests.cpp
  helpers.cpp
)
target_link_libraries(ingestion_tests
  PRIVATE
    storage
    ingestion
    file_management
    GTest::gtest
    GTest::gtest_main
    Boost::system
    nlohmann_json::nlohmann_json
)
add_test(NAME IngestionTests COMMAND ingestion_tests)
