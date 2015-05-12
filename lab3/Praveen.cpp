/***************************************************
Praveen Yalagandula
Amit K Muraleedharan
**************************************************/
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "CraigUtils.h"
#include "Params.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Random.h"
#include "Praveen.h"
#include "Window.h"

#if defined (_MSC_VER)
using namespace epl;
#endif
using namespace std;
using String = std::string;

Initializer<Praveen> __Praveen_initializer;

//int Praveen::total_num = 0 ; // sorry, no statics allowed

String Praveen::species_name(void) const
{
	return "Praveen";
}

Action Praveen::encounter(const ObjInfo& info)
{
	if (info.species == species_name()) {
		/* don't be cannibalistic */
		set_course(info.bearing + M_PI);
		return LIFEFORM_IGNORE;
	}
	else {
		hunt_event->cancel();
		SmartPointer<Praveen> me{this};
		hunt_event = new Event(0.0, [me] (void) { me->hunt(); });
		return LIFEFORM_EAT;
	}
}

void Praveen::initialize(void)
{
	LifeForm::add_creator(Praveen::create, "Praveen");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
Praveen::Praveen()
{
		hunt_event = Nil<Event>();
		SmartPointer<Praveen> me{this};
		(void) new Event(0.0, [me] (void) { me->live();});
}


Praveen::~Praveen()
{
}

void Praveen::spawn(void)
{
	Praveen* child = new Praveen;
	reproduce(child);
}


Color Praveen::my_color(void) const
{
  	return CYAN;
}  

LifeForm* Praveen::create(void)
{
	Praveen* res = Nil<Praveen>();
	res = new Praveen;
	res->display();
	return res;
}


/*
 * this event is only called once, when the object is "born"
 *
 * I don't want to call "set_course" and "set_speed" from the constructor
 * 'cause OBJECTS ARE NOT ALIVE WHEN THEY ARE CONSTRUCTED (remember!)
 * so... in the constructor I schedule this event to happen in 0 time
 * units
 */
void Praveen::live(void)
{
	set_course(drand48() * 2.0 * M_PI);
	set_speed(2 + 5.0 * drand48());
	SmartPointer<Praveen> me{this};
	hunt_event = new Event(5.0, [me] (void) { me->hunt();});
}

void Praveen::hunt(void)
{
  const String fav_food = "Algae";

  hunt_event = Nil<Event>();
  if (health() == 0) { return; }

  ObjList prey = perceive(40.0);

  double best_d = HUGE;
  int count = 0 ;
  for (ObjInfo i : prey) {
    count++ ;
    if (i.species == fav_food) {
      course_changed = 0 ;
      if (best_d > i.distance) {
        set_course(i.bearing);
        best_d = i.distance;
      }
    }
  }
  if(best_d == HUGE){
     if(course_changed == 0){
       course_changed = 1 ;
       set_course(get_course() + M_PI) ;
     }
  }
  SmartPointer<Praveen> me{this};
  hunt_event = new Event(10.0, [me] (void) { me->hunt();});

   if (health() >= 4.0) spawn();
}

