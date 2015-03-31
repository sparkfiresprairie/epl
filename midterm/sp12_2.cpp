//
//  sp12_2.cpp
//  practice
//
//  Created by Xingyuan Wang on 3/23/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//

// Our goal is pass parameter values to a function by name
class ParamProxy {
public:
    int spd;
    int dst;
    ParamProxy(void): spd{50}, dst{50} {};
};

struct SpeedPlaceHolder {
    ParamProxy operator=(int x) {
        ParamProxy a;
        a.spd = x;
        return a;
    }
};

struct DistancePlaceHolder {
    ParamProxy operator=(int x) {
        ParamProxy a;
        a.dst = x;
        return a;
    }
};

ParamProxy operator,(ParamProxy& x, ParamProxy& y ) {
    ParamProxy a;
    if (x.spd == a.spd) {
        x.spd = y.spd;
    }
    if (x.dst == a.dst) {
        x.dst = y.dst;
    }
    return x;
}

SpeedPlaceHolder speed;
DistancePlaceHolder distance;

void doit(ParamProxy param) {
    
}