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

In addition, there are some experimental features
that I find useful but not necessarily supported by
Ruby, such as `do_once`, Algebraic Effects, etc.

## Examples
See `test.cpp`.

## Dependency
If you don't use Algebraic Effects:
A compiler that supports C++11

If you use Algebraic Effects:
A compiler that supports C++2a (for type deduction)

## Installation
Drop it anywhere in your `PATH`.
Include `rubify.hpp`.
Add `rubify.cpp` to your source file for compiling.

## Coding Conventions

### Containers
`Rubify::string` derives `std::string`, and is equivalence of 
`String` in Ruby.

`Rubify::vector` derives `std::vector`, and is equivalence of 
`Array` in Ruby.

`Rubify::map` derives `std::map`, and is equivalence of 
`Hash` in Ruby.

C++ lambda is equivalence of block in Ruby.

### Member functions
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

To break or continue the iteration within a lambda, `break`
and `continue` won't work. Use `break_` and `continue_` instead.
Currently only supported in `each`. Usage in other situations
will cause an exception.

### String interpolation
A macro `_S_` is defined for string interpolation. 
`_S_( exp )` will evaluate `exp` and turn the result 
into a string. `exp`'s result type must either has `to_s()`
function defined or can be fed to stringstream.   
If you are too lazy to type the surrounding `+`
symbol, use `S_` instead.  
An example:  
`a = 1;`  
`b = 2;`  
`cout << "" S_(a) " plus " S_(b) " equals " S_(a+b) "" << endl;`  
Output:  
`1 plus 2 equals 3`

You can undefine `puts` if it's useless for you.

### Algebraic Effects
I'm actually not a super fan of FP. My knowledge about this mechanism
mainly comes from [this article](https://overreacted.io/algebraic-effects-for-the-rest-of-us/).
[And here is a Chinese translation](https://zhuanlan.zhihu.com/p/76158581).
I recommend reading the article for a better understanding of it.

There are 2 keywords in my implementation: `require_` and `provide_`.
`require_` is like `throw`. It throws a requirement that can be caught
by `provide_` block (similar to `catch`). The interface `require_(Want, name)`
throws a requirement with return type of `Want`. `name` is literally the name
of the required thing. It can be of any type.

`provide_(handler)` can catch a requirement and feed it to `handler`, which 
is a function pointer or a lambda. After calling `require_(Want, name)`, 
the first handler up in the stack that accepts 
`name` (type must be the same) and returns a value of `Want` type will catch the
thrown requirement. If the requirement can be handled, the program will jump
back to the place `require_` is called and replace the statement with the
return value. Otherwise the handler can call `return require_(Want, name);`
to throw the requirement again to upper levels. If no handler in the stack
can handle it, `name` will be thrown as an exception.

Both operations are thread safe. Each thread is provided with its own
context. When a new thread is created, nothing is derived from the parent
thread and the initial stack is empty. If you want to require something from 
another thread (e.g. worker from master), use `require_from_(thread_id, Want, name)`
and the other thread's *current* context will be used. This may be dangerous,
so be careful with it.

One example can be found in `test.cpp` where a recursive function dive into
multiple levels and require a value provided by an upper level from the bottom.

### Tips
Read comments in `real_rubify.hpp` for more details.

Some tips on using C++ if you've never used C++11 or later:

A lambda is essentially a function pointer, but can
optionally capture local context.
The most brainless way to define a lambda is like this:  
`[&]( $arguments ) -> $return_type {`  
`	$body`  
`}`  
It captures all local contexts and should work in most cases.

Try to use `auto` feature in C++. It saves you a lot of 
effort figuring out and typing the proper type name.  
For example:
`auto add = [&](int a, int b) -> int {`  
`	return a+b;`  
`};`  
`auto c = add(1, 2);`  

## Supported Functions
String interpolation
- `S_` 
- `_S_`

Short circuit
- `continue_`
- `break_`

`vector`:  
- `[]` (support negative index)
- `+`
- `+=`
- `&`
- `*`
- `take`
- `drop`
- `each` (with/without index)
- `group_by`
- `sort_by`
- `sort_by_`
- `map` (with/without index)
- `map_` (with/without index)
- `select`
- `select_`
- `zip`
- `norm_zip` (duplicate the shorter vector's elements to make them equally long, then zip)
- `flatten`
- `to_s`
- `join`
- `to_h`

`string`:
- `<` (comparison)
- `split`

`map`:
- `each`
- `to_a`
- `to_s` 

Algebraic effects
- `provide_`
- `require_`

Helper functions
- `do_once`
- `do_a_few_times`
- `do_at_interval`
- `do_a_few_times_at_interval`

TODO:
- more functions
- `Enumerator`
- conversion between types

