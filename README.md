# Rubify

C++ is great and powerful, but lacks some very basic 
functions like `string.split`, `vector.group_by`, etc.
This library allows you to write some basic operations 
in C++ like in Ruby, which has sweeeeeet syntactic sugar.

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

Anywhere in Ruby a block is required, use a C++ lambda.

If the std container already has a member function, 
(e.g., `push_back`, `pop_back`, `size`),
it won't be overridden. Otherwise, the function name and 
behavior will be the same as in Ruby. `!` character is
not allowed in C++ as identifier, so it's replaced by
`_` (underscore). `?` is not allowed either. But it incurs
no ambiguity. So it's simply removed.

Container elements are copied by value, unlike in Ruby,
where everything is an object. If you want things to
work the Ruby way, pass around pointers for everything.

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
the Ruby `puts`. If you find any of the macros conflicting
with your stuff, undefine it.

Try to use `auto` feature in C++. It saves you a lot of 
effort typing the type name.






