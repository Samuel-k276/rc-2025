CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O3 -pthread

SERVER_CPP := $(wildcard src/server/*.cpp)
CLIENT_CPP := $(wildcard src/client/*.cpp)
COMMON_CPP := $(wildcard src/common/*.cpp)

build: $(SERVER_CPP) $(CLIENT_CPP) $(COMMON_CPP)
	rm -rf ./build/*
	$(CXX) $(CXXFLAGS) -o build/ES $(SERVER_CPP) $(COMMON_CPP)
	$(CXX) $(CXXFLAGS) -o build/user $(CLIENT_CPP) $(COMMON_CPP)

run: build
	./build/ES
	./build/user

clean: build
	rm -rf ./build/*

format:
	find src -type f \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i {} \;

.PHONY: build run clean format