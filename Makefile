BUILD_DIR := build
SRC_FILES := $(shell find src -name '*.cpp')

.PHONY: all configure build test clean rebuild lint test-verbose format

all: build

configure:
	mkdir -p $(BUILD_DIR)
	conan install . --output-folder=build --build=missing
	cd $(BUILD_DIR) && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
	cd $(BUILD_DIR) && cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake

build: configure
	cmake --build $(BUILD_DIR)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

test-verbose: build
	cd $(BUILD_DIR) && ctest --verbose

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

lint: configure
	clang-tidy $(SRC_FILES) -p $(BUILD_DIR)

format: configure
	clang-format -i src/**/*.cpp

format-check: configure
	clang-format --dry-run -Werror src/**/*.cpp
