#include "rubify.hpp"

using Rubify::vector;
using Rubify::string;

/* expect: 10 1 1 2 2 */
void test_access() {
	vector<int> v = std::vector<int>( 10, 0 );
	v[6] = 1;
	v[-5] = 1;
	v[-8] = 9;
	v.each([&] (int& e) {
		e++;
	});
	puts( v.take(7).drop(2).to_s() );
}

int main() {
	test_access();
}
