BUILD_DIR := build
SRC_FILES := $(shell find src -name '*.cpp')
BUILD_TYPE ?= Release

.PHONY: all configure build test clean rebuild lint test-verbose format

all: build

configure:
	mkdir -p $(BUILD_DIR)
	conan install . --output-folder=$(BUILD_DIR) --build=missing --settings=build_type=$(BUILD_TYPE)
	cmake --preset conan-$(shell echo $(BUILD_TYPE) | tr A-Z a-z)
	ln -sf $(BUILD_DIR)/build/$(BUILD_TYPE)/compile_commands.json compile_commands.json

build: configure
	cmake --build $(BUILD_DIR)/build/$(BUILD_TYPE)

debug:
	$(MAKE) BUILD_TYPE=Debug build

debug-configure:
	$(MAKE) BUILD_TYPE=Debug configure

test: build
	cd $(BUILD_DIR)/build/$(BUILD_TYPE) && ctest --output-on-failure

test-verbose: build
	cd $(BUILD_DIR)/build/$(BUILD_TYPE) && ctest --verbose

test-filter: build
	cd $(BUILD_DIR)/build/$(BUILD_TYPE) && ctest -R '$(FILTER)'

test-filter-verbose: build
	cd $(BUILD_DIR)/build/$(BUILD_TYPE) && ctest -R '$(FILTER)' --verbose

clean:
	rm -rf $(BUILD_DIR)
	rm ./compile_commands.json

rebuild: clean all

lint: configure
	clang-tidy $(SRC_FILES) --extra-arg=-Isrc --extra-arg=-std=c++20 -p $(BUILD_DIR)/build/$(BUILD_TYPE)

format: configure
	clang-format -i src/**/*.cpp

format-check: configure
	clang-format --dry-run -Werror src/**/*.cpp
