//
//  sp12_7.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/24/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//
#include <iostream>

template <typename DS, typename FO, typename VT>
void func(DS data, FO compare, VT val) {
    for (auto i = data.begin(); i != data.end(); i++) {
        if (compare(*i, val)) {
            std::cout << *i << std::endl;
        }
    }
}
//template <typename DS, typename VT>
//struct CompThing {
//    bool operator()(DS x, VT y) { return x < y; }
//};