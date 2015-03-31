//
//  sp12_3.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/23/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

template <bool b, typename T1, typename T2> struct rtype;
template <typename T1, typename T2> struct rtype<true, T1, T2> { using type = T1; };
template <typename T1, typename T2> struct rtype<false, T1, T2> { using type = T2; };

template <typename T1, typename T2>
class Vector{
    //using X = typename choose_type<T1, T2>::type;
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

/*
 template <typename T1, typename T2>
 struct choose_type {
 static constexpr bool flag = sizeof(T1) > sizeof(T2)? true : false;
 using type = typename rtype<flag, T1, T2>::type;
 };
*/

