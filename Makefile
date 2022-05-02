# https://makefiletutorial.com
.PHONY: clean scan

SHELL=/bin/bash

CC = g++
CXXFLAGS = -O2 -std=c++20 -I .
ARMA_FLAGS = -larmadillo -lhdf5 -fopenmp
BUILD_DIR = build

all: scan

scan: scan.cpp
	$(CC) scan.cpp -o $@ $(CXXFLAGS) $(ARMA_FLAGS)

clean:
	rm -rf ./scan
