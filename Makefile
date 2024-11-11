.PHONY: all build

all: build

build:
	@mkdir -p bin
	@g++ -std=c++11 -o bin/example example.cpp -I.
