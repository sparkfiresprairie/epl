
Project2. Valarray with Expression Templates
------------------------------------------------
This project is to implement the container epl::valarray. A valarray is essentially a vector that contains values , i.e., types that have the arithmetic operators defined. 

###Features:
1. Arithmetic operations are valid. That is, it is possible to add two valarrays, and the result should (at least conceptually) be another valarray. 
2. Lazy evaluation of expressions (using expression templates) are requred.
3. A result of the appropriate type is provided when valarrays with different element types are used. 

###Function Lists:
1. push_back--inherited
2. pop_back--inherited
3. operator[] -- inherited
4. appropriate iterator and const_iterator classes (and begin/end functions) -- inherited
5. the binary operators *,/,-,+
6. a unary - (arithmetic negation)
7. all necessary functions to convert from one valarray type to another
8. a constructor that takes an initial size -- inherited
9. a sum() function that adds all elements in the valarray using standard addition
10. an accumulate function that adds all elements in the valarray using the given function object
11. an apply member function that takes a unary function argument and returns (conceptually) a new valarray where the function has been applied to each element in the valarray. Of course, this apply method must follow all the rules for lazy evaluation (i.e., it won’t return a real valarray, but rather some sort of expression template).
12. a sqrt member function that is implemented by passing a sqrt function object to the apply member function. The element type created from sqrt will either be double (for input valarrays that were int, float or double originally), or will be std::complex\<double\> for valarrays that were originally std::complex\<float\> or std::complex\<double\> originally.

###General Notes:
  1. The statement: z = x op y should set z[k] = x[k] op y[k] for all k and for any binary operator op. A reasonable subset of the binary and unary operators must be supported.
  2. If the two operands are of differing length, the length of the result should be the minimun of the lengths of the two operands (ignore values past the end of the longer operand).
  3. If one of the operands is a scalar, you should add (or whatever operation is implied) that scalar value to each of the elements in the valarray. In other words, you should implicitly expand the scalar to be a valarray of the appropriate length. But DON'T actually create this implied valarray.
  4. If x,y,z,w are valarrays, optimize the execution of statements like z = ( x * y ) + w so that only one loop gets executed and no temporary arrays are allocated.
  5. Type promotion should be to the strictest type acceptable for the operation to occur – e.g., if a valarray\<int\> is added to a complex\<float\> the result should be a valarray\<complex\<float\>\>. If a valarray<double> is added to a complex\<float\> then the result should be a valarray\<complex\<double\>\>.

Here is how I implemented [Valarray.h](./Valarray.h).
