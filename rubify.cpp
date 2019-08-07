#include "real_rubify.hpp"

namespace Rubify {

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

	template<typename Want, typename Have>
	vector< std::function<Want(Have)> > AlgebraicEffect<Want, Have>::stack;

};


