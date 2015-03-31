//
//  sp13_6.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/26/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

#include <iostream>

class A {
    virtual void doit() {};
};

class B :public A {
    virtual void doit() {};
};

int main() {
//    int a = 3;
//    const int* p = &a;
//    int* q = &a;
//    p = q;
//    q = p;
    
    A a;
    B b;
    
    A* p = &a;
    B* q = &b;
    
    p = q;
    
    std::cout << ( (void*)p != (void*)q ) << std::endl;
    
    
}
