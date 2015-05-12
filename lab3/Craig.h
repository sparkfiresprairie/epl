#if !(_Craig_h)
#define _Craig_h 1

#include <memory>
#include "LifeForm.h"
#include "Init.h"

class Craig : public LifeForm {
protected:
  static void initialize(void);
  void spawn(void);
  void hunt(void);
  void startup(void);
  Event* hunt_event;
public:
  Craig(void);
  ~Craig(void);
  Color my_color(void) const;   // defines LifeForm::my_color
  static SmartPointer<LifeForm> create(void);
  virtual std::string species_name(void) const;
  virtual Action encounter(const ObjInfo&);
  friend class Initializer<Craig>;
};


#endif /* !(_Craig_h) */
