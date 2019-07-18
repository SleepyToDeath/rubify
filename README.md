# Rubify

C++ is great and powerful, but lacks some very basic 
functions like `string.split`, `vector.group_by`, etc.
This library allows you to write them
in C++ like in Ruby. Syntactic sugar is sweeeet!

It's basically a wrapper for `std::string` and some
containers, extending them with the functionality of
their counterpart in Ruby. 

I'm conservative and trying
to stay compatible with native C++. For example, elements
are not assumed to always be pointers, and there's no 
universal parent type "Object".

## Dependency
A compiler that supports C++11

## Installation
Drop it anywhere in your `PATH`.

## Coding Conventions
`string` derives `std::string`, and is equivalence of 
`String` in Ruby.

`vector` derives `std::vector`, and is equivalence of 
`Array` in Ruby.

TODO:  
Hash  
Enumerator  
......

C++ lambda is equivalence of block in Ruby.

If the std container already has a function, 
(e.g., `push_back`), it won't be overridden(e.g. by `append`). 
Otherwise, the function name and 
behavior will be the same as in Ruby(if possible). 
`!` character is not allowed in C++ as identifier, 
so it's replaced by `_` (underscore). `?` is not allowed either. 
But it incurs no ambiguity. So it's simply removed.

Container elements are copied by value, unlike in Ruby,
where everything is an object. If you want things to
work the Ruby way, pass around pointers for everything.

Types can't be mixed up in a container, nor can elements' 
type be changed(e.g. by `map_`), unless you define everything 
as subtype of an "Object" class like in Ruby.

To break or continue within a lambda, use `break_` and
`continue_`. Currently only supported by `each`.

A macro `S_` (capital S then underscore) is defined for
string interpolation. `S_( exp )` will evaluate `exp`
and turn the result into a string. Beware it includes
the surrounding `+` symbol, and therefore must be
used within a chain of string.
An example:  
`a = 1;`  
`b = 2;`  
`cout << "" S_(a) " plus " S_(b) " equals " S_(a+b) "" << endl;`  
Output:  
`1 plus 2 equals 3`

I'm also defining a macro `puts`, which is the same as
the Ruby `puts`. If you find it conflicting with your stuff, 
undefine it.

Some tips on using C++ if you've never used C++11 or later:

A lambda is essentially a function pointer, but can
optionally derive local context.
The most brainless way to define a lambda is like this:
`
[&]( $arguments ) -> $return_type {
	$body
}
`
It derives all local contexts and should work in most cases.

Try to use `auto` feature in C++. It saves you a lot of 
effort typing the type name.






