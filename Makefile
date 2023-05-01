# https://makefiletutorial.com
.PHONY: clean scan

SHELL=/bin/bash

CC = h5c++
CXXFLAGS = -O2 -std=c++17 -I .
ARMA_FLAGS = -larmadillo -lhdf5
BUILD_DIR = ./build
BIN_DIR = $(HOME)/bin

all: cavity_scan

cavity_scan: cavity_scan.cpp
	mkdir -p $(BUILD_DIR)
	$(CC) cavity_scan.cpp -o $(BUILD_DIR)/$@ $(CXXFLAGS) $(ARMA_FLAGS)

install:
	mkdir -p $(BIN_DIR)
	cp $(BUILD_DIR)/cavity_scan $(BIN_DIR)

clean:
	rm -rf cavity_scan.o $(BUILD_DIR)
