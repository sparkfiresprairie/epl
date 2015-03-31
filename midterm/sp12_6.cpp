//
//  sp12_6.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/24/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

#include <iostream>
#include <iterator>
#include <vector>
#include <list>

template <typename T>
bool is_vector(const T& t) {return false;};

//template <typename T>
bool is_vector(typename std::random_access_iterator_tag t) {
    return true;
}

template <typename DS>
void ds_type(DS x) {
    typename std::iterator_traits<typename DS::iterator>::iterator_category tag{};
    if(is_vector(tag))
        std::cout << "vector like" << std::endl;
    else
        std::cout << "list like" << std::endl;
}

//int main() {
//    std::vector<int> a;
//    std::list<int> b;
//    ds_type(a);
//    ds_type(b);
//    return 0;
//}
