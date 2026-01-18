add_library(storage
  storage.cpp
)

target_include_directories(storage
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(storage
  PUBLIC cxx_std_20
)
