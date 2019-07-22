all: rubify.hpp test.cpp
	g++ -g -o test test.cpp -std=c++11
