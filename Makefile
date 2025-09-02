.PHONY: all build test clean rebuild

all: build

build:
	cmake -S . -B build
	cmake --build build

test: 
	ctest --test-dir build/ --output-on-failure --verbose

clean:
	rm -rf build

rebuild: clean build

