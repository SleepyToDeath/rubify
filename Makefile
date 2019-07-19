all: rubify.hpp test.cpp
	g++ -O2 -o test test.cpp -std=c++11
