# https://makefiletutorial.com
.PHONY: clean scan

SHELL=/bin/bash

CC = h5c++
CXXFLAGS = -O2 -std=c++17 -I .
ARMA_FLAGS = -larmadillo -lhdf5
DATA_DIR = data

all: scan

scan: scan.cpp
	mkdir -p $(DATA_DIR)
	$(CC) scan.cpp -o $@ $(CXXFLAGS) $(ARMA_FLAGS)

clean:
	rm -rf ./scan ./scan.o
