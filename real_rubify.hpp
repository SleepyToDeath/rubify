#ifndef REAL_RUBIFY_HPP
#define REAL_RUBIFY_HPP

#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<sstream>
#include<iostream>
#include<functional>
#include<type_traits>
#include<thread>
#include<mutex>

namespace Rubify {


/* ============== Forward Declaration ================ */
	template<typename T>
	class vector;

	template < typename KT,
           typename VT,
           typename CT = std::less<KT> > 
	class map;


/* ==================== Macro ==================== */
	#define puts(exp) std::cout<<(exp)<<std::endl
	#define errputs(exp) std::cerr<<(exp)<<std::endl

	#define _S_(exp) ( [&]()->std::string { \
		std::stringstream ss;\
		ss << (Rubify::StringFactory< decltype(exp) >::convert(exp));\
		return ss.str();\
	}() )

	#define S_(exp) +(_S_(exp))+


/* =================== String ==================== */
	class string: public std::string {
		public:

		/* inherit constructors */
		using std::string::string;

		/* Inherited copy constructor accepts subclass.
			So conversion from base class is needed. */
		string(std::string src): std::string(src) {}
	
		bool operator <(string rival) {
			return compare(rival) < 0;
		}

		/* implementation put into cpp to break dependency cycle */
		vector<string> split(std::string delimiter);

		int to_i() {
			return std::stoi(*this);
		}

		int to_i(int base) {
			return std::stoi(*this, nullptr, base);
		}


		bool start_with(std::string pre) {
			return this->find_first_of(pre) == 0;
		}

		bool end_with(std::string suf) {
			return this->find_last_of(suf) == this->length() - suf.length();
		}

	};

	/* helper type */
	template <typename T>
	class has_to_s
	{
		typedef char one;
		struct two { char x[2]; };

		/* [TODO] to be frank I don't know why `...` always has a lower priority.
			Better confirm this from some C++ document */
		template <typename ALIAS> static one test( decltype(&ALIAS::to_s) ) ;
		template <typename ALIAS> static two test(...); 

		public:
		enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};

	/* helper type */
	/*	Try every possible way to turn a type to a format
		acceptable by stringstream at compile time.
		[!] All conditions must be exclusive.
		That is, no 2 of them can be true at the same time.
		But they can all be false. In that case, the type
		can not be fed to stringstream and there is a 
		compile error.
		Beware to use ALIAS type in the condition because
		it must take effect at the point of that member
		function's specialization.
		*/
	template<typename T>
	class StringFactory{
		public:

		template<typename ALIAS = T, typename = typename std::enable_if<!has_to_s<ALIAS>::value>::type >
		static ALIAS convert(ALIAS t) {
			return t;
		}

		template<typename ALIAS = T, typename = typename std::enable_if<has_to_s<ALIAS>::value>::type >
		static string convert(ALIAS t) {
			return t.to_s();
		}
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

	inline void continue_() { throw RubifyExecption(NEXT, "Next"); }

	inline void break_() { throw RubifyExecption(BREAK, "Break"); } 

	/* simulate algebraic effect (which is actually not supported by Ruby) */
	/* [!] don't use these classes directly. ONLY use macros below */
	template<typename T>
	class PopStackHelper {
		public:
		PopStackHelper (map<std::thread::id, vector<T> >& src): stacks(src), depth(src[std::this_thread::get_id()].size()) {}
		PopStackHelper (map<std::thread::id, vector<T> >&& src) = delete;
		~PopStackHelper () {
			if (depth == stacks[std::this_thread::get_id()].size())
				stacks[std::this_thread::get_id()].pop_back();
		}
		private:
		map<std::thread::id, vector<T> >& stacks;
		int depth;
	};

	/* What a handler should do:
		Try to handle `name`, and return result.
		If unable to handle, `return require_(name)` to throw 
			the requirement	to upper level.
		If non of the handlers can handle it, eventually `name`
			will be thrown as an exception */

	/* [!] The handler will be destroyed upon the destruction
		of current BLOCK! Therefore don't write something like:
		`
		if (some condition)
			provide_( handler_1 )
		else
			provide_( handler_2 )
		a_function_requiring_some_handler();
		`
		Try this instead:
		`
		auto handler = handler_2;
		if (some condition)
			handler = handler_1;
		provide_(handler)
		a_function_requiring_some_handler();
		` 	*/
	
	template<typename Want, typename Have>
	class AlgebraicEffect {
		public:
		static map<std::thread::id, vector< std::function<Want(Have)> > >& provide(std::function<Want(Have)> handler) {
			auto id = std::this_thread::get_id();

			stack_lock.lock();
			if (stacks.count(id) == 0)
				stacks[id].clear();
			stack_lock.unlock();

			stacks[id].push_back(handler);
			return stacks;
		}

		static Want require(Have name) {
			auto id = std::this_thread::get_id();

			stack_lock.lock();
			if (stacks.count(id) == 0)
				stacks[id].clear();
			stack_lock.unlock();

			if (stacks[id].empty())
				throw name;
			auto current = stacks[id].back();
			stacks[id].pop_back();
			Want ret = current(name);
			stacks[id].push_back(current);
			return ret;
		}

		static Want require_from(std::thread::id id, Have name) {
			stack_lock.lock();
			if (stacks.count(id) == 0)
				stacks[id].clear();
			stack_lock.unlock();

			if (stacks[id].empty())
				throw name;
			auto current = stacks[id].back();
			stacks[id].pop_back();
			Want ret = current(name);
			stacks[id].push_back(current);
			return ret;
		}

		static map<std::thread::id, vector< std::function<Want(Have)> > > stacks;
		static std::mutex stack_lock;
	};

	template <typename T>
	class unary_function_traits : public unary_function_traits<decltype(&T::operator())>
	{};

	template <typename ClassType, typename ReturnType, typename Arg>
	class unary_function_traits<ReturnType(ClassType::*)(Arg) const>
	{
		public:
		typedef ReturnType result_type;
		typedef Arg arg_type;
	};

	#define __DO_CONCAT__(x, y) x##y
	#define __REAL_DO_CONCAT__(x, y) __DO_CONCAT__(x, y)

	#define provide_(handler) \
		Rubify::PopStackHelper __REAL_DO_CONCAT__(pop_stack_helper_, __LINE__)(Rubify::AlgebraicEffect< \
		Rubify::unary_function_traits<decltype(handler)>::result_type, \
		Rubify::unary_function_traits<decltype(handler)>::arg_type \
		>::provide(handler));

	template <typename T>
	class MaybeString{
		public:
		template <typename ALIAS> 
		static std::enable_if<std::is_convertible<ALIAS, string>::value, string>::type test(ALIAS name);

		template <typename ALIAS> 
		static std::enable_if<!std::is_convertible<ALIAS, string>::value, ALIAS>::type test(ALIAS name);
	};

	#define require_(Want, name) \
		Rubify::AlgebraicEffect<Want, decltype(Rubify::MaybeString<decltype(name)>::test(name))>::require(name)

	#define require_from_(thread_id, Want, name) \
		Rubify::AlgebraicEffect<Want, decltype(Rubify::MaybeString<decltype(name)>::test(name))>::require_from(thread_id, name)


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

		const T& operator[](int index) const {
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

		int locate( std::function< bool(T&) > go_before_target ) {
			int i=0;
			int j=this->size()-1;
			while (i!=j)
			{
				int k = (i+j)/2;
				if (go_before_target((*this)[k]))
					i = k + 1;
				else
					j = k;
			}
			if (go_before_target((*this)[i]))
				i++;
			return i;
		}

		/* equivalent to xxx.each do |index, element| */
		vector<T>& each( std::function< void(int, T&) > lambda ) {
			for (int i =0; i<this->size(); i++)
			{
				try 
				{
					lambda(i, (*this)[i]);
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
		vector< vector<T> > group_by( std::function< T2(const T&) > lambda) {
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
		vector<T> sort_by( std::function< T2(const T&) > lambda) {
			vector<T> cp = *this;
			cp.sort_by_(lambda);
			return cp;
		}

		template<typename T2>
		vector<T>& sort_by_( std::function< T2(const T&) > lambda) {
			std::sort(this->begin(), this->end(), [&](T a, T b) -> bool {
				return lambda(a) < lambda(b);
			});
			return *this;
		}

		template<typename T2>
		vector<T2> map( std::function< T2(int, const T&) > lambda) {
			vector<T2> ret;
			each( [&](int index, T& element){
				ret.push_back(lambda(index, element));
			});
			return ret;
		}

		vector<T>& map_( std::function< T(int, const T&) > lambda ) {
			each( [&](int index, T& element){
				element = lambda(index, element);
			});
			return *this;
		}


		template<typename T2>
		vector<T2> map( std::function< T2(const T&) > lambda) {
			vector<T2> ret;
			each( [&](T& element){
				ret.push_back(lambda(element));
			});
			return ret;
		}

		vector<T>& map_( std::function< T(const T&) > lambda ) {
			each( [&](T& element){
				element = lambda(element);
			});
			return *this;
		}

		vector<T> select( std::function< bool(const T&) > lambda ) {
			vector<T> ret;
			each( [&](T& element){
				if (lambda(element))
					ret.push_back(element);
			});
			return ret;
		}

		vector<T>& select_( std::function< bool(const T&) > lambda ) {
			vector<T> ret;
			each( [&](T& element){
				if (lambda(element))
					ret.push_back(element);
			});
			(*this) = ret;
			return *this;
		}

		vector<T> slice(int start, int length) {
			if (start < 0)
				start = this->size() + start;
			if (start < 0 || start + length > this->size() )
				throw RubifyExecption("out of bound");
			vector<T> ret;
			for (int i=start; i<start+length; i++)
				ret.push_back((*this)[i]);
			return ret;
		}


		/* 	Normalized zip.
			The shorter vector will propotionally repeat 
			its elements to make both vectors equally long.
			For example, 
			[1, 2].norm_zip([3,4,5,6])
			= [1,1,2,2].zip([3,4,5,6])
			= [[1,3],[1,4],[2,5],[2,6]] 
			*/
		vector< vector<T> > norm_zip( vector<T> another ) {
			if (this->size() > another.size())
				return another.norm_zip(*this).each([](vector<T>& pair) {
					T tmp = pair[0];
					pair[0] = pair[1];
					pair[1] = tmp;
				});
			vector< vector<T> > ret;
			int j = 0;
			for (int i=0; i<this->size(); i++)
			{
				while(j<another.size() && (j+1)*this->size() <= (i+1)*another.size())
				{
					vector<T> zipped;
					zipped.push_back( (*this)[i] );
					zipped.push_back( another[j] );
					ret.push_back(zipped);
					j++;
				}
			}
			return ret;
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

		T flatten() {
			T ret;
			each( [&](T& sub_vector) {
				sub_vector.each( [&](decltype(sub_vector[0])& element) {
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

		string to_s(int i = 0) {
			return "[ "+ this->map<string>( [&](T element) -> string {
				return _S_(element);
			}).join(", ") + " ]";
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

	/* Eat shit C++ standard committee! */
	template<>
	class vector<bool>: public std::vector<bool> {
		public:
		std::vector<bool>::reference operator[](int index) {
			if (index >= 0)
				return this->std::vector<bool>::operator [](index);
			else
				return this->std::vector<bool>::operator [](this->size() + index);
		}

		bool operator[](int index) const {
			if (index >= 0)
				return this->std::vector<bool>::operator [](index);
			else
				return this->std::vector<bool>::operator [](this->size() + index);
		}
	};

	template < typename KT,
           typename VT,
           typename CT> 
	class map: public std::map<KT, VT, CT> {

		public:

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

		/* ------------- String --------------- */
		string to_s() {
			string ret = "{ ";
			this->each( [&](KT key, VT value) {
				ret += "" S_(key) " => " S_(value)", ";
			});
			ret = ret.substr(0, ret.length()-2);
			ret += " }";
			return ret;
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


/* ================ Helper Functions ==================== */
	/* same id = same event */
	inline void do_once(int id, std::function< void(void) > something) {
		static map<int, int> counter;
		if (counter.count(id) == 0)
		{
			something();
			counter[id] = 0;
		}
	}

	inline void do_a_few_times(int id, int times, std::function< void(void) > something) {
		static map<int, int> counter;
		if (counter.count(id) == 0)
			counter[id] = 0;
		if (counter[id] < times)
		{
			something();
			counter[id]++;
		}
	}

	inline void do_at_interval(int id, int interval, std::function< void(void) > something) {
		static map<int, int> count_down;
		if (count_down.count(id) == 0)
			count_down[id] = 0;
		if (count_down[id] == 0)
			something();
		count_down[id] = (count_down[id] + 1) % interval;
	}

	inline void do_a_few_times_at_interval(int id, int times, int interval, std::function< void(void) > something) {
		static map<int, int> counter;
		static map<int, int> count_down;
		if (counter.count(id) == 0)
		{
			counter[id] = 0;
			count_down[id] = 0;
		}
		if (counter[id] < times)
		{
			if (count_down[id] == 0)
			{
				something();
				counter[id]++;
			}
			count_down[id] = (count_down[id] + 1) % interval;
		}
	}

};

#endif

