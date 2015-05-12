#if !(_Init_h)
#define _Init_h 1

template <class T>
class Initializer {
  int& count(void) { static int x = 0; return x; }
public:
  Initializer(void) { 
    if ((count())++ == 0) {
      T::initialize();
    }
  }
};



#endif /* !(_Init_h) */
