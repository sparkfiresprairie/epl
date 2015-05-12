#if !(_ObjInfo_h)
#define _ObjInfo_h 1
#include <string>

//#include "Default_ops.h"
struct ObjInfo {
  std::string species;               // species of the object
  double health;                // their health
  double distance;              // distance between us
  double bearing;               // course I can take to get where it is now
  double their_speed;           
  double their_course;          
  bool operator == (const ObjInfo& o) const {
    return species == o.species &&
      distance == o.distance &&
      bearing == o.bearing &&
      their_speed == o.their_speed &&
      their_course == o.their_course;
  };

  void copy(const ObjInfo& o) { // this is exactly what the default copy
                                // constructor would do.
    species = o.species;
    health = o.health;
    distance = o.distance;
    bearing = o.bearing;
    their_speed = o.their_speed;
    their_course = o.their_course;
  }

  ObjInfo(void) {} // use this constructor with care!
  ObjInfo(const ObjInfo& o) { copy(o); }
  ObjInfo& operator=(const ObjInfo& o) { copy(o); return *this; }
};

#endif /* !(_ObjInfo_h) */
