//
//  sp13_2.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/25/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

#include <iostream>
#include <string>
using namespace std;

//template <typename T>
//string typeString(T) { return string("unknown type"); }

class Foo {
public:
    static string myType(void) { return string("Foo"); }
};

class dFoo : public Foo {
public:
    static string myType(void) { return string("dFoo"); }
};

//template <typename T>
//string typeString(T t) {
//    string s{"pointer to "};
//    s.append(typeString(*t));
//    return s;
//}


template <typename T>
string typeString(T) {
    string s{T::myType()};
    return s;
}

template <typename T>
string typeString(T* t) {
    string s{"pointer to "};
    return s.append(typeString(*t));
}

template <typename T>
string typeString(T** t) {
    string s{"pointer to pointer to"};
    return s.append(typeString(**t));
}


template <> string typeString(int) { return string("int"); }
template <> string typeString(double) { return string("double"); }
template <> string typeString(float) { return string("float"); }


template <typename T>
void fun(const T& x) {
    string s = typeString(x);
    cout << "x is type: " << s << endl;
}

//template <typename T>
//bool isTemp(const T&) {
//    return false;
//}

template <typename T>
bool isTemp(T&) {
    return false;
}

template <typename T>
bool isTemp(T&&) {
    return true;
}

int main() {
    int a[3][5];
    //    int** a;
    fun(a);
    
    float b;
    fun(b);
    
    Foo foo;
    fun(foo);
    
    dFoo dfoo;
    Foo* p;
    p = &dfoo;
    fun(p);
    
    int x, y;
    int *q;
    
    cout << isTemp(42) << endl;
    cout << isTemp(x + y) << endl;
    cout << isTemp(new int) << endl;
    cout << isTemp(x) << endl;
    cout << isTemp(q) << endl;
}


