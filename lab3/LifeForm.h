#if ! (_LifeForm_h)
#define _LifeForm_h 1

#include <cassert>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <functional>
#ifdef _MSC_VER
# include <time.h>
#else
#include <sys/time.h>
# endif /* end #IF for Windows/Linux time.h file */

#include "Params.h"
#include "Point.h"
#include "SmartPointer.h"


/* forward declarations */
class LifeForm;
class istream;
struct ObjInfo;
typedef std::vector<ObjInfo> ObjList;
template <typename Obj> class QuadTree;

/* 
 * The map will contain IstreamCreators for LifeForms
 * The map will be keyed on a String.  This String
 * must be a keyword that uniquely identifies a type of LifeForm
 * (e.g., "Lion", "Tiger", "Bear", oh my).
 */
using IstreamCreator = std::function<SmartPointer<LifeForm>(void)>;

typedef std::map<std::string, IstreamCreator> LFCreatorTable;

/*
 * The Canvas class is something we can draw on
 */
class Canvas;

/*
 * We draw with Colors
 */
#include "Color.h"

class Event;

enum Action {
  LIFEFORM_IGNORE,
  LIFEFORM_EAT
};

class LifeForm : public ControlBlock {
private:
	/* space is the global storage that represents the 2-dimensional simulation area */
    static QuadTree<SmartPointer<LifeForm>> space;


    /* In order to perform the graphics output and to keep track of
     * which species have become extinct, we need a mechanism to scan
     * all of the LifeForms that are alive. I'm creating a std::vector<LifeForm*>
     * to hold pointers to every LifeForm. Objects insert themselves (*this)
     * into the vector in the LifeForm constructor, and remove themselves
     * from the vector in their destructor. The name of the vector is all_life
     *
     * There are two caveats
     *
     * 1. some lifeforms may be dead, yet their destructors may not yet
     * have been run (e.g., student species can create lots of LifeForm objects
     * by just calling "new Craig[1000]"). So, we have an is_alive flag that
     * will be false in LifeForms that are outside of the simulation. The all_life
     * vector will include pointers to all LifeForms (alive or dead).
     * 2. removing LifeForm objects from the vector is facilitated by having each LifeForm
     * remember its position in the vector. When we remove LifeForm (e.g,. LifeForm #10)
     * we simply replace that position with a pointer to the last LifeForm in the vector
     * and then pop_back the LifeForm at the end. The vector_pos data member tells each
     * LifeForm object where it can find this in the all_life vector
     *
     */
      static std::vector<LifeForm*> all_life;
      uint32_t vector_pos;

      /* istream_creators is a map, indexed by strings, and returning functions
       * the functions create the correct subtype of LifeForm
       * i.e., istream_creators["Craig"] returns a function. If you call that
       * function, then the function returns a Craig* allocated on the heap
       *
       * to avoid race conditions related to the order that global variables
       * and static data members are created, the implementation requires
       * that we use the syntax
       * 	istream_creators()["Craig"]
       * the istream_creators function return a reference to the actual map.
       */
      static LFCreatorTable& istream_creators(void);


      static int scale_x(double); // scale_x and scale_y are used to position the pixel
      static int scale_y(double); // in the window when drawing a LifeForm
      void print_position(void) const; // print and print_position are provided for debugging purposes
      void print(void) const;

      double energy;
      bool is_alive;

      Event* border_cross_event;    // pointer to the event for the next encounter with a boundary
      void border_cross(void);		// the event handler function for the border cross event

      void region_resize(void);		// the callback function for region resizes (invoked by the quadtree)

      Point pos;
      double update_time;           // the time when update_position was 
                                //   last called
      double reproduce_time;        // the time when reproduce was last called
      double course;
      double speed;

      Point start_point;			// start_point is sometimes used by the test program(s)
								// you can (and should) ignore it


      void resolve_encounter(SmartPointer<LifeForm>);
      void eat(SmartPointer<LifeForm>);
      void age(void);               // subtract age_penalty from energy
      void gain_energy(double);
      void update_position(void);   // calculate the current position for
				    // an object.  If less than Time::tolerance
                                // time units have passed since the last
                                // call to update_position, then do nothing
                                // (we can't have moved very far so there's
                                // no point in updating our position)

      void check_encounter(void);   // check to see if there's another object
				    // within encounter_distance.  If there's
                                // an object nearby, invoke resove_encounter
                                // on ourself with the closest object
  
      void die(void);          // kill the current life form


      void compute_next_move(void); // a simple function that creates the next border_cross_event

      ObjInfo info_about_them(SmartPointer<LifeForm>);

      const Point& position() const { return pos; }

      static Canvas win;
protected:
      double health(void) const {
    	  if (!is_alive) { return 0.0; }
    	  else { return energy / start_energy; }
      }
      void set_course(double);
      void set_speed(double);
      double get_course(void) const { return course; }
      double get_speed(void) const { return speed; }
      void reproduce(SmartPointer<LifeForm>);
      ObjList perceive(double);

public:
      LifeForm(void);
      virtual ~LifeForm(void);

      static void add_creator(IstreamCreator, const std::string&);
      static void create_life();
      /* draw the lifeform on 'win' where x,y is upper left corner */
      virtual void draw(int, int) const;
      virtual Color my_color(void) const = 0;

      void display(void) const;
      static void redisplay_all(void);
      static void clear_screen(void);

      virtual Action encounter(const ObjInfo&) = 0;
      virtual std::string species_name(void) const = 0;
      virtual std::string player_name(void) const;

friend class Algae;

/*
 * the following functions are used by the test program(s) and should not be used by students (except, of course,
 * during testing, feel free to write your own test programs)
 */
    bool confirmPosition(double xpos, double ypos) {
    	return pos.distance(Point(xpos,ypos)) < 0.10;
    }

    static void runTests(void);
    static bool testMode;
    static void place(std::shared_ptr<LifeForm>, Point p);

};

#endif /* !(_LifeForm_h) */
