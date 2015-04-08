Engineering Programming Language Project Description
====================================================
Instructor: Prof. Craig Chase

Engineering Programming Language (EPL) is a graduate software design and implementation class that emphasizes engineering level insight into the mechanics of contemporary programming languages. EPL makes extensive use of the C++ language (C++11 standard), in part because C++ is sufficiently low level that the mechanics by which object-oriented programming are implemented become readily visible to C++ programmers. Topics covered in EPL change annually, but often include, Template Meta Programming, Object Oriented Design, Design for Performance, Memory Management and Garbage Collection.

Project1. Vector Container Basics and Extensions
------------------------------------------------
###Part1 - Basics

This project is to implement the container vector. A vector is a data structure that provides O(1) random access (via an operator[]), and amortized constant append (via push_back). Our vector differs from the standard library vector in three minor ways. We are not required to implement all of the C++ standard functions, our operator[] performs bounds checking, and we have to implement a “push_front” and “pop_front”.

####Method List:

- vector(void) – creates an array with some minimum capacity (8 is fine) and length equal to zero. Must not use T::T(void). In fact, as long as Vector::Vector(int) is not called, we shall never use T::T(void)
- explicit Vector(uint64_t n) – create an array with capacity and length exactly equal to n. Initialize the n objects using T::T(void). As a special case, if n is equal to zero, you can duplicate the behavior of vector(void).
- copy and move constructors for vector<T> arguments
- copy and move assignment operators for vector<T> arguments
- destructor
- uint64_t size(void) const – return the number of constructed objects in the vector
- T& operator[](uint64_t k) – if k is out of bounds (equal to or larger than length), then we will throw std::out_of_range(“subscript out of range”). If k is in bounds, then we return a reference to the element at position k
- const T& operator[](uint64_t k) const – same as above, except for constants.
- void push_back(const T&) – add a new value to the end of the array, using amortized doubling if the array has to be resized. Copy construct the argument.
- void push_back(T&&) – same as above, but move construct the argument.
- void push_front(const T&) and void push_front(T&&) -- similar to push_back, but add the element to the front of the Vector
- void pop_back(void) and (B) void pop_front(void) – if the array is empty, throw a std::out_of_range exception (with any reasonable string error message you want). Otherwise, destroy the object at the end (or beginning) of the array and update the length. This action will not reallocate storage, even if the vector becomes empty. It is possible that a vector can have available capacity at both the front and back simultaneously. 

####Note:

1. We implement amortized doubling whenever we increase the allocated space of your vector.
2. If a call to pop_back succeeds (does not throw an exception) and is followed immediately by a call to push_back, then we will not perform any memory allocations. More generally, if two or more consecutive calls to pop_back succeed and are followed by an equal number of calls to push_back, then no allocations are performed. Similarly if one or more successive calls to pop_front succeed and are followed immediately by an equal number of push_front calls, then no memory allocations are performed.


###Part2 - Extensions

In this project we are asked to add some new functionality to our Vector<T> template class, making it more compliant with the C++ standard, and applying some of the additional knowledge we’ve gained about templates.

####Requirement:

- Create a random-access iterator type for our Vector.
- Write an emplace_back variadic member template function for our vector that constructs the object in place.
- Create a member template constructor that takes an iterator pair b and e and initializes the Vector to contain copies of the values from [b, e).
- Create a constructor that will initialize a Vector from a std::initializer_list<T>

####Note:
1. For iterators, specific requirements are as follows

- Provide begin/end fuctions for our Vector.
- Provide both iterator and const_iterator (and begin/end for each).
- Ensure that an iterator can be converted (without warnings or type casts) to const_iterator. 
- Ensure that const_iterator cannot be converted to iterator.
- Design our iterator so that it throws the exception epl::invalid_iterator whenever the value of an invalid iterator is used. “Using the value” of an iterator includes comparison operations (with other iterators), dereferencing the iterator, incrementing the iterator, etc. Assigning to an iterator, for example, is not “using the value” of the iterator, it is assigning a new value to the iterator (and should not throw an exception).
- epl::invalid_iterator has three severity levels: (a) If the iterator references a position that no longer exists (i.e., the old position is out-of-bounds), the exception you throw must use the level SEVERE. (b) If the iterator reference a position that is in-bounds, but the memory location for that position may have been changed (e.g., a reallocation has been performed because of a push_back, or a new assignment has been performed to the Vector), then the exception you throw must have the level MODERATE. (c) If the iterator is invalidated for any other reason, the exception must have the level MILD.

For test purpose, all the staffs are squeezed into [Vector.h](./lab1/Vector.h) file.

Project2. Valarray with Expression Templates
------------------------------------------------
This project is to implement the container epl::valarray. A valarray is essentially a vector that contains values , i.e., types that have the arithmetic operators defined. 

###Features:
1. Arithmetic operations are valid. That is, it is possible to add two valarrays, and the result should (at least conceptually) be another valarray. 
2. Lazy evaluation of expressions (using expression templates) are requred.
3. A result of the appropriate type is provided when valarrays with different element types are used. 

###General Notes:
  1. The statement: z = x op y should set z[k] = x[k] op y[k] for all k and for any binary operator op. A reasonable subset of the binary and unary operators must be supported.
  2. If the two operands are of differing length, the length of the result should be the minimun of the lengths of the two operands (ignore values past the end of the longer operand).
  3. If one of the operands is a scalar, you should add (or whatever operation is implied) that scalar value to each of the elements in the valarray. In other words, you should implicitly expand the scalar to be a valarray of the appropriate length. But DON'T actually create this implied valarray.
  4. If x,y,z,w are valarrays, optimize the execution of statements like z = ( x * y ) + w so that only one loop gets executed and no temporary arrays are allocated.
  5. Type promotion should be to the strictest type acceptable for the operation to occur – e.g., if a valarray<int> is added to a complex<float> the result should be a valarray<complex<float>>. If a valarray<double> is added to a complex<float> then the result should be a valarray<complex<double>>.








