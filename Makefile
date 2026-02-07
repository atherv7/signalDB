BUILD_DIR := build
SRC_FILES := $(shell find src -name '*.cpp')

.PHONY: all configure build test clean rebuild lint test-verbose format

all: build

configure:
	mkdir -p $(BUILD_DIR)
	conan install . --output-folder=$(BUILD_DIR) --build=missing
	cmake --preset conan-release
	ln -sf $(BUILD_DIR)/build/Release/compile_commands.json compile_commands.json

build: configure
	cmake --build $(BUILD_DIR)/build/Release

test: build
	cd $(BUILD_DIR)/build/Release && ctest --output-on-failure

test-verbose: build
	cd $(BUILD_DIR)/build/Release && ctest --verbose

clean:
	rm -rf $(BUILD_DIR)
	rm ./compile_commands.json

rebuild: clean all

lint: configure
	clang-tidy $(SRC_FILES) --extra-arg=-Isrc --extra-arg=-std=c++20 -p $(BUILD_DIR)/build/Release

format: configure
	clang-format -i src/**/*.cpp

format-check: configure
	clang-format --dry-run -Werror src/**/*.cpp test/**/*.cpp
