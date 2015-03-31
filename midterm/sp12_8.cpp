//
//  sp12_8.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/24/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

#include <iostream>
#include <typeinfo>

template <typename T1, typename T2>
struct F {
};


template<typename T3, typename T4>
void doit(F<T3, T4> f) {
    std::cout << "T3's type: " << typeid(T3).name() << std::endl;
    std::cout << "T4's type: " << typeid(T4).name() << std::endl;
}

int main() {
    doit(F<int, double>{});
}