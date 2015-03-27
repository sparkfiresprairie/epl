/*
 * Quiz1.h
 *
 *  Created on: Mar 11, 2015
 *      Author: chase
 */

#ifndef QUIZ1_H_
#define QUIZ1_H_

namespace epl_quiz1 {

template <typename Expr>
struct assignment;

/* the IntExpr is a simple wrapper that promotes an integer literal (or constant)
 * into something that can appear on the right-hand-side of an assignment
 * For example _X = 42
 * the 42 is an int, the assignment operator for a Var converts the 42 into an IntExpr
 * and returns an assignment struct.
 * Having a IntExpr wrapper allows us to generalize to more complicated right-hand-sides
 * like _X = _Y + _Z
 * where the right hand side is some kind of an expression, i.e., something that has
 * an operator() that we can call
 * This description will make more sense when you look at the assignment struct
 */
struct IntExpr {
	int val;
	IntExpr(int v) : val{v} {}
	int operator()(void) { return val; }
};

struct Var {
	int val;

	template <typename Expr>
	assignment<Expr> operator=(Expr e);

	assignment<IntExpr> operator=(int e);

	/* this operator makes it possible to print a Var, e.g.,
	 * cout << _X will work. My Print functionality relies on this capability
	 * If you re-implement Print or provide an equivalent capability, you can
	 * eliminate this conversion operator
	 */
	operator int(void) const { return val; }
};

template<typename Expr>
struct assignment {
	Var& var;
	Expr e;

	assignment(Var& v, Expr ex) : var{v}, e{ex} {}
	void operator()(void) {
		var.val = e();
	}
};

template <typename Expr>
assignment<Expr> Var::operator=(Expr e) {
	return assignment<Expr>{*this, e};
}

assignment<IntExpr> Var::operator=(int v) {
	return assignment<IntExpr>{*this, v};
}

Var _X{}, _Y{}, _Z{};


/* this is the functionality I used for Print
 * You are not required to use this implementation.
 *
 * Please note that the functionality below is not sufficient to
 * print expressions. For example
 * 		Print(_X + _Y)
 * will not work in my solution (and is not required for Quiz1)
 * However, I can do
 *     _Z = _X + _Y, Print(_Z)
 *
 */
// Here print is a proxy.
template <typename T>
struct print {
	T x;
    // Why here using const&?
	print(T const& _x) : x{_x} {}
	void operator()(void) { cout << x << endl; }
};

/* simple Prints like Print(42) */
template <typename T>
print<T> Print(const T& v) { return print<T>{v}; }

/* special overload for Var because Var must be captured by reference
 * -- if we make a copy of the Var won't be able to see when the original variable
 * is changed via an assignment
 */
inline print<Var&> Print(Var& v) { return print<Var&>{v}; }

/* special overload for const char* because of some C++ obscurity that doesn't
 * like deducing T as const char when given an argument like "Hello World"
 * If you're curious what the problem is, comment out this overload and try
 * to compile part A of the quiz
 */
print<char const*> Print(char const* v) { return print<char const*>{v}; }

/* End functionality for Print */

template <typename... TYPES>
struct program;
    
template <typename T>
struct program<print<T>> {
    print<T> _val;
    program(print<T> val) : _val(val) {}
    void operator()(void) {
        _val();
    }
};
    
template <typename T, typename... Tails>
struct program<T, Tails...> {
    T _val;
    program<Tails...> _rest;
    program(const T& val, const Tails&... rest): _val{val}, _rest{rest...} {}
    void operator()(void) {
        _val();
        _rest();
    }
};
    

template <typename T>
program<T> Program(const T& t) { return program<T>(t); }
    

template <typename... Args>
program<Args...> Program(const Args&... args) {
    return program<Args...>(args...);
}
    
} // end namespace epl_quiz1

#endif /* QUIZ1_H_ */
