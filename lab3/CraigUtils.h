#ifndef _CraigUtils_h
#define _CraigUtils_h 1
#include <cassert>

template <class T>
class Nil {
public:
  operator T* () { return static_cast<T*>(0); }
};

template <class T>
void Delete(T*& x) {
  delete x;
  x = Nil<T>();
}


#define MAXFLOAT (1.0e127)
#endif /* _CraigUtils_h */
