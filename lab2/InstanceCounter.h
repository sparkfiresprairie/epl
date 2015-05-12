
#pragma once

#ifndef _InstanceCounter_h
#define _InstanceCounter_h

class InstanceCounter {
public:
        static int counter;
        InstanceCounter(void) {
                ++counter;
        }
};

#endif /* _InstanceCounter_h */