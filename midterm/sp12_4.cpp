//
//  sp12_4.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/23/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//
#include <iostream>

template <bool b, typename T1, typename T2> struct rtype;
template <typename T1, typename T2> struct rtype<true, T1, T2> { using type = T1; };
template <typename T1, typename T2> struct rtype<false, T1, T2> { using type = T2; };

template <typename H, typename Tail>
struct List {
    using type = typename rtype<(sizeof(H)>sizeof(Tail)), H, Tail>::type;
};

template <typename H, typename T1, typename T2>
struct List<H, List<T1, T2>> {
    using type = typename rtype<(sizeof(H)>sizeof(typename List<T1,T2>::type)), H, typename List<T1,T2>::type>::type;
};

template <typename T1, typename T2>
class Vector{
public:
    using X = typename rtype<(sizeof(T1)>sizeof(T2)), T1, T2>::type;
    X* data;
    int length;
public:
    explicit Vector(int sz) {
        length = sz;
        data = new X[sz];
    }
    
    T1& operator[](int k) { return *((T1*)(&data[k]));}
};

template <typename T1, typename H, typename Tail>
class Vector<T1, List<H, Tail>> {
public:
    using X = typename rtype<(sizeof(T1)>sizeof(typename List<H, Tail>::type)), T1, typename List<H, Tail>::type>::type;
};

//int main() {
//    std::cout << "longest:" << sizeof(typename Vector<int,List<int,List<double,bool>>>::X) << std::endl;
//}

