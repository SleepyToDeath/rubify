#ifndef RUBIFY_HPP
#define RUBIFY_HPP

	#include "real_rubify.hpp"

	template<typename Want, typename Have>
	extern Rubify::map<std::thread::id, Rubify::vector< std::function<Want(Have)> > > Rubify::AlgebraicEffect<Want, Have>::stacks;

	template<typename Want, typename Have>
	extern std::mutex Rubify::AlgebraicEffect<Want, Have>::stack_lock;

#endif

