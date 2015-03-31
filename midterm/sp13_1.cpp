//
//  sp13_1.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/24/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

#include <iostream>

class Fibonacci {
    int64_t val1;
    int64_t val2;
    int64_t size;
public:
    Fibonacci(int64_t n): val1{1}, val2{1}, size{n} {}
    class iterator {
    private:
        uint64_t* position = nullptr;
    public:
        iterator(void): position{&val1} {}
        int64_t& operator*(void) { return val1; }
        
    };
    
};
int main(void) {
    for (int64_t v : Fibonacci(20)) {
        std::cout << v << std::endl;
    }
}