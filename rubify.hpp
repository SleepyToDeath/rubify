#ifndef RUBIFY_HPP
#define RUBIFY_HPP

#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<sstream>
#include<iostream>
#include<functional>

namespace Rubify {

/* ======================== Definition ===========================*/

	#define puts(exp) std::cout<<(exp)<<std::endl

	#define _S_(exp) ( [&]()->std::string { \
		std::stringstream ss;\
		ss << (exp);\
		return ss.str();\
	}() )

	#define S_(exp) +(_S_(exp))+

	enum RubifyExecptionType { ERROR, NEXT, BREAK };
	
	template<typename T>
	class vector;

	class string: public std::string {
		public:

		string():std::string() {
		}

		string( std::string src):std::string(src) { 
		}

		string( const char* src ):std::string(src) {
		}

		bool operator <(string rival) {
			return compare(rival) > 0;
		}

		/* implementation moved below to break dependency cycle */
		vector<string> split(std::string delimiter);
	};

	class RubifyExecption {
		public:
		RubifyExecption(string msg) {
			type = ERROR;
			this->msg = msg;
		}

		RubifyExecption(RubifyExecptionType type, string msg) {
			this->type = type;
			this->msg = msg;
		}

		RubifyExecptionType type;
		string msg;
	};

	void continue_ () {
		throw RubifyExecption(NEXT, "Next");
	}

	void break_ () {
		throw RubifyExecption(BREAK, "Next");
	}

	template<typename T>
	class vector: public std::vector<T> {
		public:

		vector():std::vector<T>() {
		}

		vector( std::vector<T> src):std::vector<T>(src) { 
		}

		T& operator[](int index) {
			if (index >= 0)
				return this->std::vector<T>::operator [](index);
			else
				return this->std::vector<T>::operator [](this->size() + index);
		}

		vector<T> take(size_t count) {
			if (count > this->size())
				throw RubifyExecption("Taking too much!\n");
			vector<T> ret;
			for (int i=0; i<count; i++) {
				ret.push_back( (*this)[i] );
			}
			return ret;
		}

		vector<T>& each( std::function< void(T&) > lambda ) {
			for (int i=0; i<this->size(); i++)
			{
				try 
				{
					lambda((*this)[i]);
				}
				catch (RubifyExecption e)
				{
					if (e.type == ERROR)
						throw e;
					else if (e.type == BREAK)
						break;
					else
						continue;
				}
			}
			return (*this);
		}

		template<typename T2>
		vector< vector<T> > group_by( std::function< T2(T) > lambda) {
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

		/* '<' operator must be defined for T2 */
		template<typename T2>
		vector<T> sort_by( std::function< T2(T) > lambda) {
			vector<T> cp = *this;
			cp.sort_by_(lambda);
			return cp;
		}

		template<typename T2>
		vector<T>& sort_by_( std::function< T2(T) > lambda) {
			std::sort(this->begin(), this->end(), [&](T a, T b) -> bool {
				return lambda(a) < lambda(b);
			});
			return *this;
		}

		template<typename T2>
		vector<T2> map( std::function< T2(T) > lambda) {
			vector<T2> ret;
			each( [&](T& element){
				ret.push_back(lambda(element));
			});
			return ret;
		}

		vector<T>& map_( std::function< T(T&) > lambda ) {
			each( [&](T& element){
				element = lambda(element);
			});
			return *this;
		}

	
		/* If lengths are different, the function will 
			only zip until the end of the shorter one.
			The rest are DROPPED */
		vector< vector<T> > zip( vector<T> buddy ) {
			size_t len = this->size() < buddy.size() ? this->size() : buddy.size();
			vector< vector<T> > ret;
			for (int i=0; i<len; i++)
			{
				vector<T> zipped;
				zipped.push_back( (*this)[i] );
				zipped.push_back( buddy[i] );
				ret.push_back(zipped);
			}
			return ret;
		}

		vector<T> flatten() {
			vector<T> ret;
			each( [&](T& sub_vector) {
				sub_vector.each( [&](T& element) {
					ret.push_back(element);
				});
			});
			return ret;
		}

		string join(std::string delimiter) {
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

		string to_s() {
			return this->map<string>( [&](T element) -> string {
				return _S_(element);
			}).join(" ");
		}

	};

/*
	template<T>
	class vector< vector<T> >: public std::vector< vector<T> > {
		vector<T> flatten() {
			vector<T> ret;
			each( [&](T& sub_vector) {
				sub_vector.each( [&](T& element) {
					ret.push_back(element);
				})
			})
			return ret;
		}
	}

	template<>
	class vector<string>: public std::vector<string> {
		public:
		string join(std::string delimiter) {
			if (size() == 0)
				return std::string("");
			string ret = (*this)[0];
			for (int i=1; i<size(); i++)
			{
				ret += delimiter;
				ret += (*this)[i];
			}
			return ret;
		}
	};

	*/


/* ======================== Implementation ===========================*/


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

