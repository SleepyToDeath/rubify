#ifndef RUBIFY_HPP
#define RUBIFY_HPP

#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<sstream>
#include<iostream>
#include<functional>
#include<type_traits>

namespace Rubify {



/* ==================== Macro ==================== */
	#define puts(exp) std::cout<<(exp)<<std::endl

	#define _S_(exp) ( [&]()->std::string { \
		std::stringstream ss;\
		ss << (exp);\
		return ss.str();\
	}() )

	#define S_(exp) +(_S_(exp))+


/* ============== Forward Declaration ================ */
	template<typename T>
	class vector;

	template < typename KT,
           typename VT,
           typename CT = std::less<KT> > 
	class map;

/* =================== String ==================== */
	class string: public std::string {
		public:

		/* inherit constructors */
		using std::string::string;

		/* Inherited copy constructor accepts subclass.
			So conversion from base class is needed. */
		string(std::string src): std::string(src) {}
	
		bool operator <(string rival) {
			return compare(rival) > 0;
		}

		/* implementation moved below to break dependency cycle */
		vector<string> split(std::string delimiter);
	};

/* ================= Exception =================== */
	enum RubifyExecptionType { ERROR, NEXT, BREAK };
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
		throw RubifyExecption(BREAK, "Break");
	}


/* ================= Container =================== */
	template<typename T>
	class vector: public std::vector<T> {
		public:

		/* ------------- Constructor -------------- */
		/* inherit constructors */
		using std::vector<T>::vector;

		vector(std::vector<T> src): std::vector<T>(src) {}

		vector(T array[]): std::vector<T>(array, array+sizeof(array)/sizeof(T)) {}


		/* --------------- Access ----------------- */
		T& operator[](int index) {
			if (index >= 0)
				return this->std::vector<T>::operator [](index);
			else
				return this->std::vector<T>::operator [](this->size() + index);
		}

		vector<T> drop(size_t count) {
			vector<T> ret;
			if (count > this->size())
				return ret;
			for (int i=count; i<this->size(); i++) {
				ret.push_back( (*this)[i] );
			}
			return ret;
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
			for (auto it=this->begin(); it!=this->end(); it++)
			{
				try 
				{
					lambda(*it);
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

		/* -------------- Interact -------------- */
		vector<T> operator &(vector<T> another) {
			auto exist = this->zip( vector(this->size(), true) ).to_h();
			vector<T> ret;
			another->each( [&] (T& e) {
				if (exist.count(e) > 0)
					ret.push_back(e);
			});
			return ret;
		}

		vector<T>& operator +=(vector<T> another) {
			another.each( [&](T& e) {
				this->push_back(e);
			});
			return (*this);
		}

		vector<T> operator +(vector<T> another) {
			auto ret = (*this);
			another.each( [&](T& e) {
				ret.push_back(e);
			});
			return ret;
		}

		string operator *(string delimiter) {
			return this->join(delimiter);
		}

		vector<T> operator *(size_t times) {
			vector<T> ret;
			for (int i=0; i<times; i++)
				ret += (*this);
			return ret;
		}


		/* ------------- Reorganize -------------- */
		template<typename T2>
		vector< vector<T> > group_by( std::function< T2(T) > lambda) {
			vector< vector<T> > ret;
			Rubify::map< T2, vector<T> > boxes;
			this->each( [&] (T& value) {
				T2 key = lambda(value);
				boxes[key].push_back(value);
			});
			boxes.each( [&]( T2 k, vector<T> v) {
				ret.push_back(v);
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
		vector< vector<T> > zip( vector<T> another ) {
			size_t len = this->size() < another.size() ? this->size() : another.size();
			vector< vector<T> > ret;
			for (int i=0; i<len; i++)
			{
				vector<T> zipped;
				zipped.push_back( (*this)[i] );
				zipped.push_back( another[i] );
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


		/* ------------- String --------------- */
		string join(std::string delimiter) {
			if (this->size() == 0)
				return std::string("");
			string ret = _S_( (*this)[0] );
			for (int i=1; i<this->size(); i++)
			{
				ret += delimiter;
				ret += _S_( (*this)[i] );
			}
			return ret;
		}

		string to_s() {
			return this->map<string>( [&](T element) -> string {
				return _S_(element);
			}).join(" ");
		}


		/* ------------- Conversion --------------- */
		template<typename T2>
		Rubify::map<std::enable_if<std::is_same<T, vector<T2> >::value , T2>, std::enable_if<std::is_same<T, vector<T2> >::value , T2 > > to_h() {
			Rubify::map<T2, T2> ret;
			this->each( [&] (T& pair) {
				ret[pair[0]] = pair[1];
			});
			return ret;
		}

	};



	template < typename KT,
           typename VT,
           typename CT> 
	class map: public std::map<KT, VT, CT> {

		/* ------------- Constructor -------------- */
		using std::map<KT, VT, CT>::map;

		map(std::map<KT, VT, CT> src): std::map<KT, VT, CT>(src) {}

		/* --------------- Access ----------------- */
		Rubify::map<KT, VT, CT>& each( std::function< void(KT, VT&) > lambda ) {
			for (auto it=this->begin(); it != this->end(); it++)
			{
				try 
				{
					lambda(it->first, it->second);
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


		/* ------------- Conversion --------------- */
		vector< vector<VT> > to_a() {
			vector< vector<VT> > ret;
			this->each( [&] (VT k, VT v) {
				vector<VT> pair;
				pair.push_back(k);
				pair.push_back(v);
				ret.push_back(pair);
			});
			return ret;
		}
   };


/* ======================== Delayed Implementation ===========================*/


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

