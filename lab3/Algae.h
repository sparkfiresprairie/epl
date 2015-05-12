#if !(_Algae_h)
#define _Algae_h 1
#include <memory>

#include "LifeForm.h"
#include "Init.h"

class Algae : public LifeForm {
  static void initialize(void);
  Event* photo_event;
  void photosynthesize(void);
public:
  Algae(void);
  void draw(int,int) const;     // defines LifeForm::draw
  Color my_color(void) const;   // defines LifeForm::my_color
  std::string species_name(void) const;
  std::string player_name(void) const;
  virtual Action encounter(const ObjInfo&);
  static SmartPointer<LifeForm> create(void);
  static void create_spontaneously(void);
  friend class Initializer<Algae>;
};

#endif /* !(_Algae_h) */
