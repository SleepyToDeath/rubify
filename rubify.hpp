#ifndef RUBIFY_HPP
#define RUBIFY_HPP

	#include "real_rubify.hpp"

	template<typename Want, typename Have>
	extern Rubify::vector< std::function<Want(Have)> > Rubify::AlgebraicEffect<Want, Have>::stack;

#endif

