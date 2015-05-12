#if !(_Cons_h)
#define _Cons_h 1

#include <assert.h>

template <class T>
struct Cons {
  T car;
  Cons* cdr;
  Cons(const T& x) : car(x) { cdr = 0; }
  Cons(const Cons<T>& c) : car(c.car) {
    if (c.cdr) cdr = new Cons(*c.cdr);
    else cdr = 0;
  }
  ~Cons(void) { delete cdr; }

private:

  Cons& operator=(const Cons<T>&) {
    /* don't assign Cons's! */
    assert(0);
    return *this;
  }
};
#endif !(_Cons_h)
