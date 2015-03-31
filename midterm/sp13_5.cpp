//
//  sp13_5.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/24/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

#include <iostream>
#include <utility>
class HotPotato {
public:
    ~HotPotato(void) { std::cout << "burned\n"; }
};
class Container {
    HotPotato* pot;
public:
    Container(void) { pot = new HotPotato; }
};

Container pass(Container& c) { return std::move(c); }

int main(void) {
    Container c;
    Container p;
    for(int k = 0; k < 10; k += 1) {
        c = pass(p);
    }
}
