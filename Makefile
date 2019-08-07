all: rubify.hpp test.cpp
	g++ -g -o test rubify.cpp test.cpp -std=c++2a
