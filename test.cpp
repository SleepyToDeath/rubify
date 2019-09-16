#include "rubify.hpp"

using Rubify::vector;
using Rubify::string;
using Rubify::map;
using Rubify::continue_;
using Rubify::break_;
using Rubify::do_once;
using Rubify::do_at_interval;
using Rubify::do_a_few_times;
using Rubify::do_a_few_times_at_interval;

/* expect:
[ 10, 1, 1, 2, 2 ] 
7
*/
void test_access() {
	vector<int> v = std::vector<int>( 10, 0 );
	v[6] = 1;
	v[-5] = 1;
	v[-8] = 9;
	v.each([&] (int& e) {
		e++;
	});
	puts( v.take(7).drop(2).to_s() );

	v.sort_by_<int>([](const int& e)->int { return e; });
	puts(v.locate( [](int& e)->bool { return e < 2; } ) );
	puts(v.locate( [](int& e)->bool { return e <= 11; } ) );
}

/* expect:
[ 0, 0, 0, 6, 8, 10, 12, 14, 16, 0 ] */
void test_break_n_continue() {
	vector<int> v = std::vector<int>(10, 0);
	v.each( [&] (int i, int& e) {
		if (i<3)
			continue_();
		if (i>8)
			break_();
		e = i*2;
	});
	puts(v.to_s());
}

/* expect:
{ 0 => 1, 1 => 3, 2 => 9, 3 => 27 }
[ [ 0, 1 ], [ 1, 3 ], [ 2, 9 ], [ 3, 27 ] ]
[ 0, 1, 1, 3, 2, 9, 3, 27 ] */
void test_conversion() {
	map<int, int> m;
	m[0] = 1;
	m[1] = 3;
	m[2] = 9;
	m[3] = 27;
	puts(m.to_s());
	puts(m.to_a().to_s());
	puts(m.to_a().flatten().to_s());

	vector<int>mask(3,0);
	mask[1] = 1;
	mask[2] = 2;
	puts(m.to_a().flatten().norm_zip(mask).to_s());
}

struct People {
	string name;
	int zip;
	string email;

	string to_s() {
		return "(" + name + "," S_(zip) "," + email + ")";
	}
};

/* expect:
(Alice,19026,alice@gmail.com)
(Bob,19026,bob@gmail.com)
(Cate,19026,cate@gmail.com)
(David,19026,david@gmail.com)

(Anna,19104,anna@gmail.com)
(Bran,19104,bran@gmail.com)
(Chloe,19104,chloe@gmail.com)
(Delphi,19104,delphi@gmail.com)

(Abby,19520,abby@gmail.com)
(Black,19520,black@gmail.com)
(Cyan,19520,cyan@gmail.com)
(Doge,19520,doge@gmail.com)
(Emma,19520,emma@gmail.com) 
[ (Bran,19104,bran@gmail.com), (Anna,19104,anna@gmail.com), (Chloe,19104,chloe@gmail.com), (Delphi,19104,delphi@gmail.com) ]*/
void test_group_n_sort() {
	vector<People> name_list;
	name_list.push_back( {"Bob", 19026, "bob@gmail.com"} );
	name_list.push_back( {"Bran", 19104, "bran@gmail.com"} );
	name_list.push_back( {"Emma", 19520, "emma@gmail.com"} );
	name_list.push_back( {"Cate", 19026, "cate@gmail.com"} );
	name_list.push_back( {"David", 19026, "david@gmail.com"} );
	name_list.push_back( {"Anna", 19104, "anna@gmail.com"} );
	name_list.push_back( {"Black", 19520, "black@gmail.com"} );
	name_list.push_back( {"Doge", 19520, "doge@gmail.com"} );
	name_list.push_back( {"Chloe", 19104, "chloe@gmail.com"} );
	name_list.push_back( {"Delphi", 19104, "delphi@gmail.com"} );
	name_list.push_back( {"Abby", 19520, "abby@gmail.com"} );
	name_list.push_back( {"Cyan", 19520, "cyan@gmail.com"} );
	name_list.push_back( {"Alice", 19026, "alice@gmail.com"} );

	puts( 
		name_list
		.sort_by<string>( [&](People p)->string {return p.name;})
		.group_by<int>( [&](People p)->int {return p.zip;})
		.map<string>( [&](vector<People> v)->string { return v.join("\n");} )
		.join("\n\n")
	);

	puts( name_list.select([&](const People& p)->bool {return p.zip == 19104;}).to_s() );
	
}

/* expect:
123
5 
*/
void test_stoi()
{
	puts(string("123").to_i());
	puts(string("101").to_i(2));
}


void test_debug()
{
	for (int i=0; i<30; i++)
	{
		puts("[" S_(i) "]");
		do_once( 1, [&]() { std::cout<<"| do once! |"; } );
		do_a_few_times( 2, 5, [&]() { std::cout<<"| do 5 times! |"; } );
		do_at_interval( 3, 3, [&]() { std::cout<<"| do at interval 3! |"; } );
		do_a_few_times_at_interval( 4, 5, 5, [&]() { std::cout<<"| do 5 times at interval 5! |"; } );
		puts(" ");
	}
}

void need_int(int lvl)
{
	if (lvl == 0)
	{
		int seven = require_(int, 7);
		puts(seven);
	}
	else
	{
		provide_([&](int name)->int {
			if (name == lvl)
				return lvl;
			else
				return require_(int, name);
		} )
		need_int(lvl - 1);
	}
}

string arabic_to_english[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven"};

void need_string(int lvl)
{
	if (lvl == 0)
	{
		string eleven = require_from_(std::this_thread::get_id(), string, "11");
		puts(eleven);
	}
	else
	{
		provide_([&](string name)->string {
			if (name == _S_(lvl))
				return arabic_to_english[lvl];
			else
				return require_(string, name);
		} )
		need_string(lvl - 1);
	}
}

/* expected output:
7
eleven
*/
void test_algebraic_effect()
{
	need_int(10);
	need_string(20);
}

int main() {
	test_access();
	test_break_n_continue();
	test_conversion();
	test_group_n_sort();
	test_stoi();
	test_debug();
	test_algebraic_effect();
}

