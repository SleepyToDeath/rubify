#include "rubify.hpp"

using Rubify::vector;
using Rubify::string;

int main() {
	vector<int> v = std::vector<int>( 10, 0 );
	v[-2] = 1;
	v[-5] = 1;
	v[-8] = 9;
	puts( v.take(7).to_s() );
}
