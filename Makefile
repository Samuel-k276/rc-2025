SERVER_CPP := $(wildcard src/server/*.cpp)
CLIENT_CPP := $(wildcard src/client/*.cpp)
COMMON_CPP := $(wildcard src/common/*.cpp)

build: $(SERVER_CPP) $(CLIENT_CPP) $(COMMON_CPP)
	rm -rf ./build/*
	g++ -pthread -o build/ES $(SERVER_CPP) $(COMMON_CPP)
	g++ -o build/user $(CLIENT_CPP) $(COMMON_CPP)

run: build
	./build/ES
	./build/user

clean: build
	rm -rf ./build/*

.PHONY: build run clean