BUILD_DIR := build

.PHONY: all configure build test clean rebuild

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
