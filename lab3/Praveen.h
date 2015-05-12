#if !(_Praveen_h)
#define _Praveen_h 1

#include "LifeForm.h"
#include "Init.h"

class Praveen : public LifeForm {
protected:
  int course_changed ;
  static void initialize(void);
  void spawn(void);
  void hunt(void);
  void live(void);
  Event* hunt_event;
public:
  Praveen(void);
  ~Praveen(void);
  Color my_color(void) const;   // defines LifeForm::my_color
  static LifeForm* create(void);
  std::string species_name(void) const;
  Action encounter(const ObjInfo&);
  friend class Initializer<Praveen>;
};


#endif /* !(_Praveen_h) */
