build: src/server/main.cpp src/client/main.cpp
	rm -rf ./build/*
	g++ -o build/ES src/server/main.cpp
	g++ -o build/user src/client/main.cpp

run: build
	./build/ES
	./build/user

clean: build
	rm -rf ./build/*

.PHONY: build run clean