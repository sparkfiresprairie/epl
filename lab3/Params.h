#if !(_Params_h)
#define _Params_h 1

#include <cmath>
#include <algorithm>
#define ALGAE_SPORES 1

#if defined(_AIX) && !defined(XLC_IS_STUPID) && !defined(__GNUG__)
#define XLC_IS_STUPID
#endif /* _AIX */

#include "SimTime.h"

/*****************************************************/
/* PLEASE SEE Params.cpp FOR ACTUAL PARAMETER VALUES */
/*****************************************************/

/*
 * any time you successfully eat something, you pay this cost
 * NOTE: if the object you eat has 5 energy (or less) then you'll
 * lose energy by trying to eat them
 */
double eat_cost_function(double notUsed0=0, double notUsed1=0);

/*
 * if you attempt to eat an object, then your probability of success
 * is determined by this function.
 * e1 is the energy of the eater
 * e2 is the energy of the food
 *
 * NOTE: if I'm trying to eat you, you can also try to eat me.
 * the simulator must ensure that we both don't succeed
 * One way to do this, is to choose one LifeForm and let it attempt
 * to eat the other.  If it fails, you can allow the food to try and be
 * the eater.
 */
double eat_success_chance(double e1, double e2);

/* time between when you eat and when you get the energy */
extern const SimTime digestion_time;

/* If you eat an object with E energy, then after digestion you gain
   eat_efficiency * E more energy */
extern const double eat_efficiency;

/* the amount of energy a life form starts with */
extern const double start_energy;

/* 
 * it costs energy to exist, stationary, isolated objects eventually die
 * you should schedule an event every age_frequency time units
 * the event should subtract age_penalty units of energy from the LifeForm
 * if the energy drops below min_energy, the LifeForm should die
 */
extern const double age_penalty;
extern const double age_frequency; // 0.1 unit of energy per unit time

/* whether you eat or not, you take a penalty for colliding */
extern const double encounter_penalty;

/* the cost to move is non-linear */
double movement_cost(double speed, double time);

/* all life forms must have at least this much energy, or they die */
extern const double min_energy;

/*
 * when a LifeForm reproduces, the child must be placed no further than
 * reproduce_dist units away.
 * The child should be given 1/2 the energy of the parent
 * and then both child and parent should be charged the reproduce_cost
 * NOTE: reproduce cost is a percentage, so the penalty
 * is energy * reproduce_cost
 */
extern const double reproduce_dist;
extern const double reproduce_cost;  // a fraction
extern const double min_reproduce_time;
/*
 * Algae gain energy automatically
 * Every algae_photo_time time units, an Algae gains Algae_energy_gain
 * units of energy
 */
extern const double Algae_energy_gain;
extern const SimTime algae_photo_time;

/*
 * two objects whos' centers are encounter_distance away (or closer)
 * are considered to have collided.
 * you are required to eventually simulate a collision when objects move
 * towards each other
 *
 * you do not need to simulate a collision everytime objects are close
 * if you miss collisions because objects to not cross boundaries in the
 * QuadTree, that is OK.
 * BUT if two objects move towards each other, and do not eat each other
 * and continue to move towards each other, you must simulate enough
 * events so that the objects eventually die (from the encounter_penalty
 * being applied over and over again)
 *
 * You must also correctly simulate the case that two objects move
 * towards each other, collide once, and then turn and go opposite
 * directions.  (this case is very hard, solve it last)
 */
extern const double encounter_distance;

/*
 * every time an object attempts to look around, it should be assessed this
 * penalty.
 */
double perceive_cost(double radius);

/* objects must not be permitted to move faster than max_speed
 * if they do, then their speed should be set to max_speed (do not
 * kill them for trying)
 */
extern const double max_speed;

/* objects should not be permitted to percieve more than max_perceive_range
 * or perceive less than min_percieve_range.
 * If they do, adjust their perceive range to the appropriate bound
 * (do not kill them for trying)
 */
extern const double max_perceive_range;
extern const double min_perceive_range;

extern const int grid_max;
extern const int win_x_size;
extern const int win_y_size;

// minimum time between scheduling an
// event and when that event can occur
extern const double min_delta_time; 

/*
 * You may ignore the parameters after this line.  Just extra
 * stuff I added to the solution
 */

/* how to resolve encounters when both objects want to eat */
enum EncounterResolver {
  EVEN_MONEY,                   // flip a coin
  BIG_GUY_WINS,                 // big guy gets first bite
  UNDERDOG_IS_HERE,             // let the little guy have a chance
  FASTER_GUY_WINS,              // speeding eagle gets the mouse
  SLOWER_GUY_WINS               // ambush!
};

/*
 * set your encounter resolution strategy here.  Only affects the
 * case where both objects want to eat each other and both objects
 * "succeed"
 */
extern const EncounterResolver encounter_strategy;

enum SimulationTerminationStrategy {
  RUN_TILL_HALF_EXTINCT,
  RUN_TILL_ONE_SPECIES_LEFT,
  RUN_TILL_EVENTS_EXHAUSTED     // runs forever if ALGAE_SPORES is on
};

extern const SimulationTerminationStrategy termination_strategy;

#endif /* !(_Params_h) */
