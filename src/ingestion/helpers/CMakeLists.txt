add_library(helpers STATIC
  helpers.cpp
  helpers.h
)

target_link_libraries(helpers PRIVATE
  nlohmann_json::nlohmann_json
  storage
)

target_include_directories(helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
