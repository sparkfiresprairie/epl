/*
 * Source.cpp
 *
 *  Created on: Mar 11, 2015
 *      Author: chase
 */

#include <iostream>
#include <cstdint>

using std::cout;
using std::cin;
using std::endl;

#include "Quiz1.h"

using namespace epl_quiz1;

/*
 * This quiz gradually increases the complexity required to build
 * a "programming language" interpreter using techniques similar to
 * expression templates.
 *
 * The examples in this quiz use a very small subset of a reasonable interpreter.
 * On a larger scale, the expression templates might support something like this:
 *
 * auto my_prog = Program(_X = 1, _Y = 1
 *     While(_X < 10,
 *        Print(_Y),
 *        _X = _X + 1,
 *        _Y = _Y * _X
 *     )
 *  )
 *
 *  the statement above would create a variable called "prog", that variable is a function
 *  object that we can run. When the function object is executed, this particular
 *  function would print out the first 10 values of N! (factorial)
 *
 *  prog(); // prints the first ten factorial numbers
 *
 *  OK, so that's the concept for this quiz. We are keeping the examples simple for
 *  your implementation. I'm also providing some starter code that you should consider
 *  using in your solution. If you use the stater code, then I am likely to understand what
 *  you are trying to do. If you do not use my starter code, then I may not understand
 *  what you are trying to do, and if what you are trying to do fails, I may not be able
 *  to give any partial credit for your attempt.
 *
 *  Write code to solve parts A through D below. Put your solution in Quiz1.h
 *  while you can make changes to this file, any changes you make to this file,
 *  and any other files you create, will be ignored. Only your Quiz1.h file will be
 *  evaluated.
 *
 *  For partial credit, your solution does not need to compile.
 *
 *  YOU WILL NEED TO COMMENT OUT PARTS B-D WHEN YOU FIRST START WORKING ON PART A
 *  IF YOU WANT PART A TO COMPILE
 *
 *  A correct solution compiles and runs all four parts (part A through part D)
 */


/*
 * For PartA you must demonstrate lazy evaluation
 * The function below reads input from the keyboard. The function must
 * print "Hello World" if and only if "y" is entered on the keyboard
 */
void partA(void) {
    // Print("Hello World"): return print<T>, here T is const char *;
    auto prog = Program(
                        Print("Hello World")
                        );
    
    cout << "do you want to run program A? (y/n)";
    char c;
    cin >> c;
    // Victor:
    // The class that object prog belongs to has a overload operator(). When triggered, it will
    // print out the contents stored
    if (c == 'y') { prog(); }
}

/*
 * PartB requires that you develop a solution for multi-statement programs
 * There are at least two reasonable approaches to this problem.
 * Method 1) use expression templates and overload the "," (comma) operator
 * Method 2) use variadic templates (e.g., typename...)
 *
 * Select one of these methods (or develop your own) that allows an arbitrary
 * number of statements to be part of a program.
 */


void partB(void) {
    auto prog = Program(
                        Print("Line 1"),
                        Print("Line 2"),
                        Print("Line 3")
                        );
    
    cout << "do you want to run program B? (y/n)";
    char c;
    cin >> c;
    if (c == 'y') { prog(); }
}


/*
 * Part C introduces variables, expressions and assignment statements
 * You'll find three placeholder global variables defined in quiz1.h you can
 * assume that these are the only variables that will be used (i.e., I'm not looking
 * for you to make a general solution for variables in this quiz). Hence there
 * will only be at most three variables in any program and those variables are
 * _X _Y and _Z
 * The partC function below only uses _X
 */
// _X is a var, _X = 42 returns assignment<IntExpr>. This assignment<IntExpr> object
// has a val = 42 and a overloaded (), which will return its val.
void partC(void) {
    auto prog = Program(
                        _X = 42,
                        Print(_X) // prints 42
                        );
    
    cout << "do you want to run program C? (y/n)";
    char c;
    cin >> c;
    if (c == 'y') { prog(); }
}

/* partD extends partC slightly
 * In addition to using variables _X, _Y and _Z, partD uses expressions with
 * integer literals and expressions with two variables.
 * To keep your solution short and simple, you can assume the following about expressions
 * 1) the only expression operator you need to support is operator+
 * 2) the left-hand-side operand will always be a variable
 *    _X + 1 is legal
 *    1 + _X is not something you need to support
 *    _X + _Y is legal
 *    _X + _Y + 1 is not something you need to support
 * 3) as suggested above, you only need to support simple expressions with a single +
 */

void partD(void) {
    auto prog = Program(
                        _X = 42,
                        Print(_X), // prints 42
                        _Y = _X + 1,
                        Print(_Y),  // prints 43
                        _Z = _X + _X,
                        Print(_Z) // prints 84
                        );
    
    cout << "do you want to run program D? (y/n)";
    char c;
    cin >> c;
    if (c == 'y') { prog(); }
}



int main(void) {
    partA();
    partB();
    partC();
    partD();
}

