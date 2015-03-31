//
//  sp13_3.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/25/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

#include <iostream>



class Base {
public:
    virtual void doit(void) { std::cout << "TRUE\n"; }
    void foo(void) { doit(); }
};

class Derived: public Base {
    int x;
public:
    virtual void doit(void) { std::cout << "FALSE\n"; }
};

Base* array = new Derived[10];

template <typename T> void tfun1(T x) { x.doit(); }

template <typename T> void tfun2(T x) { x.foo(); }

int main() {
    array -> doit();
    array -> foo();
    (array+1) -> doit();
    array[0].doit();
    array[1].doit();
    auto a = *array;
    (*array).doit();
    a.doit();
    tfun1(*array);
    tfun2(*array);
    return 0;
}