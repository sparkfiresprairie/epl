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
    Container(void) { 
        pot = new HotPotato; 
        std::cout << "Container()" << std::endl;
    }
    Container(const Container& that) {
        pot = new HotPotato;
        *pot = *(that.pot);
        std::cout << "Container(T&)" << std::endl;
    }
    Container(Container&& that) {
        pot = that.pot;
        that.pot = nullptr;
        std::cout << "Container(T&&)" << std::endl;
    }
    Container& operator=(Container&& that) {
        if (this != &that) {
            this->pot = that.pot;
            that.pot = nullptr;
        }
        std::cout << "=(&&)" << std::endl;
        return *this;
    }
    Container& operator=(const Container& that) {
        if (this != &that) {
            *pot = *(that.pot);
        }
        std::cout << "=(&)" << std::endl;
        return *this;
    }
    ~Container() {
        delete pot;
        std::cout << "~Container()" << std::endl;
    }
};

Container pass(Container& c) { 
    std::cout << "pass" << std::endl;
    return std::move(c); 
}

int main(void) {
    Container c;
    for(int k = 0; k < 10; k += 1) {
        c = pass(c);
    }
}
