BUILD_DIR := build
SRC_FILES := $(shell find src -name '*.cpp')

.PHONY: all configure build test clean rebuild lint

all: build

configure:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

build: configure
	cmake --build $(BUILD_DIR)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

lint: configure
	clang-tidy $(SRC_FILES) -p $(BUILD_DIR)
