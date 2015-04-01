//
//  sp13_4.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/26/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

#include <iostream>
#include <vector>
#include <list>
#include <typeinfo>


template <typename T>
struct CompThing {
    bool operator()(const T& x, const T& y) { return x < y; }
};

template <typename I, typename C = CompThing<typename std::iterator_traits<I>::value_type>>
typename std::iterator_traits<I>::value_type findMax(I ib, I ie, C f) {
    using T = typename std::iterator_traits<I>::value_type;
    T max = *ib;
    for (auto i = ib; i != ie ; i++) {
        if (f(max, *i)) {
            max = *i;
        }
    }
    return max;
}

template <typename I>
typename std::iterator_traits<I>::value_type findMax(I ib, I ie) {
    using value_type = typename std::iterator_traits<I>::value_type;
    return findMax(ib, ie, CompThing<value_type>{});
}

int main() {
    int a[] = {3,1,0,0,1,0,4,6,4,2};
    std::vector<double> v{3.14, 2.71, 0, 1.414, 1.732};
    std::list<float> l{3.14, 2.71, 0, 1.414, 1.732};
    
    auto amax = findMax(a, a+(sizeof(a)/sizeof(*a)));
    std::cout << typeid(amax).name() << ":" << amax << std::endl;
    auto vmax = findMax(v.begin(), v.end());
    std::cout << typeid(vmax).name() << ":" << vmax << std::endl;
    auto lmax = findMax(l.begin(), l.end());
    std::cout << typeid(lmax).name() << ":" << lmax << std::endl;
}
