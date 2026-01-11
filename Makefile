.PHONY: build
build: clean
	@g++ storage.hpp storage.cpp main.cpp -o storage

.PHONY: run
run: build
	@./storage

.PHONY: clean
clean:
	@-rm ./storage
