#ifndef RUBIFY_HPP
#define RUBIFY_HPP

#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<sstream>
#include<iostream>

namespace Rubify {

#define puts(exp) std::cout<<exp<<std::endl

#define S_(exp) +( [&]()->std::string { \
	std::stringstream ss;\
	ss << (exp);\
	return ss.str();\
}() )+

	class string;

	template<typename T>
	class vector: public std::vector<T> {
		public:

		vector<T>& each( void lambda(T&) );

		template<typename T2>
		vector< vector<T> > group_by( T2 lambda(T&) );

		template<typename T2>
		vector<T> sort_by( T2 lambda(T& element) ) {
		}

		template<typename T2>
		vector<T> sort_by_( T2 lambda(T& element) ) {
		}

		template<typename T2>
		vector<T2> map( T2 lambda(T& element) ) {
		}

		template<typename T2>
		vector<T2> map_( T2 lambda(T& element) ) {
		}

	};

	template<>
	class vector<string>: public std::vector<string> {
		public:
		string join(std::string delimiter);
	};

	class string: public std::string {
		public:
		string( std::string src);
		vector<string> split(std::string delimiter);
	};

	template<typename T>
	vector<T>& vector<T>::each( void lambda(T&) ) {
		for (int i=0; i<this->size(); i++)
			lambda((*this)[i]);
		return (*this);
	}

	template<typename T>
	template<typename T2>
	vector< vector<T> > vector<T>::group_by( T2 lambda(T&) ) {
		vector< vector<T> > ret;
		std::map< T2, vector<T> > boxes;
		for (int i=0; i<this->size(); i++)
		{
			T value = (*this)[i];
			T2 key = lambda(value);
			boxes[key].push_back(value);
		}
		for_each(boxes.begin(), boxes.end(), [&](std::pair< T2, vector<T> > p) {
			ret.push_back(p.second);
		});
		return ret;
	}

	string::string( std::string src) { 
		this->std::string::operator =(src);
	}

	string vector<string>::join(std::string delimiter) {
		if (this->size() == 0)
			return std::string("");
		string ret = (*this)[0];
		for (int i=1; i<this->size(); i++)
		{
			ret += delimiter;
			ret += (*this)[i];
		}
		return ret;
	}

	vector<string> string::split(std::string delimiter) {
		string remain = *this;
		vector<string> ret;
		while (remain.find(delimiter) != std::string::npos)
		{
			ret.push_back(remain.substr(0, remain.find(delimiter)));
			remain.erase(0, remain.find(delimiter) + delimiter.length());
		}
		ret.push_back(remain);
		return ret;
	}

};

#endif

