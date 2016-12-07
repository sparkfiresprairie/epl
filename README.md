Advanced C++ Programming Project
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
- copy and move constructors for vector\<T\> arguments
- copy and move assignment operators for vector\<T\> arguments
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

In this project we are asked to add some new functionality to our Vector\<T\> template class, making it more compliant with the C++ standard, and applying some of the additional knowledge we’ve gained about templates.

####Requirement:

- Create a random-access iterator type for our Vector.
- Write an emplace_back variadic member template function for our vector that constructs the object in place.
- Create a member template constructor that takes an iterator pair b and e and initializes the Vector to contain copies of the values from [b, e).
- Create a constructor that will initialize a Vector from a std::initializer_list\<T\>

####Note:
1. For iterators, specific requirements are as follows

- Provide begin/end fuctions for our Vector.
- Provide both iterator and const_iterator (and begin/end for each).
- Ensure that an iterator can be converted (without warnings or type casts) to const_iterator. 
- Ensure that const_iterator cannot be converted to iterator.
- Design our iterator so that it throws the exception epl::invalid_iterator whenever the value of an invalid iterator is used. “Using the value” of an iterator includes comparison operations (with other iterators), dereferencing the iterator, incrementing the iterator, etc. Assigning to an iterator, for example, is not “using the value” of the iterator, it is assigning a new value to the iterator (and should not throw an exception).
- epl::invalid_iterator has three severity levels: (a) If the iterator references a position that no longer exists (i.e., the old position is out-of-bounds), the exception you throw must use the level SEVERE. (b) If the iterator reference a position that is in-bounds, but the memory location for that position may have been changed (e.g., a reallocation has been performed because of a push_back, or a new assignment has been performed to the Vector), then the exception you throw must have the level MODERATE. (c) If the iterator is invalidated for any other reason, the exception must have the level MILD.

Here is how I implemented [Vector.h](./lab1/Vector.h).

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

Here is how I implemented [Valarray.h](./lab2/Valarray.h).

Project3. LifeForm Simulation
------------------------------------------------
This project is to complete the simulation infrastructure that we get from the instructor and also to invent at least one new LifeForm and simulate the LifeForm's existance (or evolution) of that LifeForm.

###Overview:
  1. Events: We use the make_procedure function to create function objects as described in class. Events can then be dynamically allocated. The Event constructor takes a time and a procedure. The procedure will be called that many time units into the future (the time is relative, not absolute). Creating an event automatically puts the event in the central event queue. Events automatically delete themselves when they occur. If we delete an event before it occurs it automatically removes itself from the event queue.
  2. Region of space(quadtree): We use the QuadTree\<LifeForm*\> data structure to represent space. 
  3. How to move around:
- LifeForm::update_position(void) - computes the new position, charge for energy consumed.
- LifeForm::border_cross(void) - movement event handler function. It calls update_position and then schedules the next movement event.
- LifeForm::region_resize(void) - this function will be a callback from the QuadTree. When another object is created nearby, the object needs to determine the next possible time that they could collide. The QuadTree knows when objects are inserted inside it, so it can invoke this callback function on your objects. We have to have region_resize cancel any pending border crossing events, update the position, and schedule the next movement event (since the region has changed size, we will encounter the boundary at a different time than before.)
- LifeForm::set_course and LifeForm::set_speed - These functions should cancel any pending border_cross event, update the current position of the object and then schedule a new border_cross event based on the new course and speed. Note that an object that is stationary will never cross a border.
  4. Creation of LifeForms: LifeForms cannot be created spontaneously and just expect to be simulated. There are two ways that LifeForms can be created. First, they can be created by the LifeForm::create_life method during initialization. The other way that objects can be created is by calling reproduce.

###Rules of the Game
  1. Encounter: If two creatures ever get within one unit of distance of each other, they have an enounter. The encounter method returns either EAT or IGNORE. Since there are two LifeForms colliding, there are four cases; IGNORE/IGNORE, EAT/IGNORE, IGNORE/EAT and EAT/EAT. Provided at least one LifeForm attempts to eat, we need to generate a random number and compare the result to eat_success_chance(eater-\>energy, eatee-\>energy).
  2. Names: There are two functions, player_name and species_name. By default, player_name returns the species name. The player name is used during the contest to select the winning LifeForms among all students in the class. The player_name is used only by the contest and only to print out which LifeForms are surviving (and which have become extinct). The species_name is used when objects collide with or perceive each other. The species_name can be anything we'd like. (We can lie to enemies in the species_name!!)
  3. Perception: The perceive routine can be invoked by any derived LifeForm class at any time. max_perceive_range and min_perceive_range define the maximum and minimum radii for the perception circle. Each time an object attempts to perceive, it should be charged the perceive_cost. Note that perceive_cost is a function of the radius. The perceive function should return a list of ObjInfos. Each ObjInfo includes the name, bearing, speed, health and distance to the object. One ObjInfo should be placed into the list for every object within the prescribed radius.
  4. Eating: Objects eat each other according to their desire (indicated by the return value of encounter) and their eat_success_chance (defined in Params.h). If object A wants to eat object B, then the simulator should generate a random number and compare it to the eat_success_chance(A, B). If the random number is less than the eat success chance, then A gets to eat B. B could also be trying to eat A at the same time. Params.h includes a variable called encounter_strategy which explains how to break the tie. Once we've figured out who got to eat whom, we have to award the victor the spoils. The energy is awarded to the eatee after exactly digestion_time time units have passed (create an event). The energy awarded is reduced by the eat_efficiency multiplier. (Since eat_efficiency is less than 1, it is usually not a good idea to eat your own young!!)
  5. Aging: Objects must eventually die if they don't eat. This is true even if they act like rocks and don't move and don't perceive. Not all LifeForms are born at the same time (some are children of other LifeForms) and so they shouldn't be penalized at the same time.
  6. Reproduction: When a LifeForm reproduces, the energy from the parent LifeForm is divided in half. This amount of energy is given to both the parent and the child. Then, the fractional reproduce_cost is subtracted from both the parent and the child. For example, a LifeForm with 100 energy that reproduces will produce a child that has 50 * (1.0 - reproduce_cost) energy. The parent will have the same energy as the child. As a special rule, do not allow an object to reproduce faster than min_reproduce_time.
  
Here is how I implemented [LifeForm.cpp](./lab2/LifeForm.cpp).
