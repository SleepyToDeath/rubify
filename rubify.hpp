#ifndef RUBIFY_HPP
#define RUBIFY_HPP

#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<sstream>
#include<iostream>

namespace Rubify {

/* ======================== Definition ===========================*/

	#define puts(exp) std::cout<<(exp)<<std::endl

	#define S_(exp) +( [&]()->std::string { \
		std::stringstream ss;\
		ss << (exp);\
		return ss.str();\
	}() )+

	enum RubifyExecptionType { ERROR, NEXT, BREAK };

	class string: public std::string {
		public:
		string( std::string src) { 
			this->std::string::operator =(src);
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
			this->msg = msg
		}

		RubifyExecption(RubifyExecptionType type, string msg) {
			this->type = type;
			this->msg = msg;
		}

		RubifyExecptionType type;
		string msg;
	}

	void continue_ () {
		throw RubifyExecption(NEXT, "Next");
	}

	void break_ () {
		throw RubifyExecption(BREAK, "Next");
	}

	template<typename T>
	class vector: public std::vector<T> {
		public:

		vector( std::vector<T> src) { 
			this->std::vector<T>::operator =(src);
		}

		T& operator[](int index) {
			if (index >= 0)
				return this->std::vector<T>::operator [](index);
			else
				return this->std::vector<T>::operator [](size() + index);
		}

		vector<T> take(size_t count) {
			vector<T> ret;
			for (int i=0; i<count; i++) {
				ret.push_back( (*this)[i] );
			}


		template<typename T>
		vector<T>& each( void lambda(T& element) ) {
			for (int i=0; i<size(); i++)
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
		vector< vector<T> > group_by( T2 lambda(T element) ) {
			vector< vector<T> > ret;
			std::map< T2, vector<T> > boxes;
			for (int i=0; i<size(); i++)
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
		vector<T> sort_by( T2 lambda(T element) ) {
			vector<T> cp = *this;
			cp.sort_by_(lambda);
			return cp;
		}

		template<typename T2>
		vector<T>& sort_by_( T2 lambda(T element) ) {
			std::sort(begin(), end(), [&](T a, T b) -> bool {
				return lambda(a) < lambda(b);
			}
			return *this;
		}

		template<typename T2>
		vector<T2> map( T2 lambda(T element) ) {
			vector<T2> ret;
			each( [&](T& element){
				ret.push_back(lambda(element));
			})
			return ret;
		}

		vector<T>& map_( T lambda(T& element) ) {
			each( [&](T& element){
				element = lambda(element);
			})
			return *this;
		}

	
		/* If lengths are different, the function will 
			only zip until the end of the shorter one.
			The rest are DROPPED */
		vector< vector<T> > zip( vector<T> buddy ) {
			size_t len = size() < buddy.size() ? size() : buddy.size();
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


	};

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

