all: rubify.hpp test.cpp
	g++-9 -g -o test rubify.cpp test.cpp -std=c++2a
