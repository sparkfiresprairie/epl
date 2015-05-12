#include "Params.h"

/*
 * any time you successfully eat something, you pay this cost
 * NOTE: if the object you eat has 5 energy (or less) then you'll
 * lose energy by trying to eat them
 */
double eat_cost_function(double, double)
{
  return 5.0;
}

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
double eat_success_chance(double e1, double e2)
{
  return std::max(.10, e1 / (e1 + e2));
}

/* time between when you eat and when you get the energy */
const SimTime digestion_time = 5.0;

/* If you eat an object with E energy, then after digestion you gain
   eat_efficiency * E more energy */
const double eat_efficiency = 0.95;

/* the amount of energy a life form starts with */
const double start_energy = 100.0;

/* 
 * it costs energy to exist, stationary, isolated objects eventually die
 * you should schedule an event every age_frequency time units
 * the event should subtract age_penalty units of energy from the LifeForm
 * if the energy drops below min_energy, the LifeForm should die
 */
const double age_penalty = 10;
const double age_frequency = 100; // 0.1 unit of energy per unit time

/* whether you eat or not, you take a penalty for colliding */
const double encounter_penalty = 5.0;

/* the cost to move is non-linear */
double movement_cost(double speed, double time)
{
  return 0.01 * pow(speed, 1.5) * time;
}


/* all life forms must have at least this much energy, or they die */
const double min_energy = 1.0 + eat_cost_function(1.0, 1.0);

/*
 * when a LifeForm reproduces, the child must be placed no further than
 * reproduce_dist units away.
 * The child should be given 1/2 the energy of the parent
 * and then both child and parent should be charged the reproduce_cost
 * NOTE: reproduce cost is a percentage, so the penalty
 * is energy * reproduce_cost
 */
const double reproduce_dist = 5.0;
const double reproduce_cost = 0.05;  // a fraction
const double min_reproduce_time = 1.0;
/*
 * Algae gain energy automatically
 * Every algae_photo_time time units, an Algae gains Algae_energy_gain
 * units of energy
 */
const double Algae_energy_gain = 2.0;
const SimTime algae_photo_time = 5.0;

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
const double encounter_distance = 1.0;

/*
 * every time an object attempts to look around, it should be assessed this
 * penalty.
 */

double perceive_cost(double radius) {
  return radius / 20.0;
}
  
/* objects must not be permitted to move faster than max_speed
 * if they do, then their speed should be set to max_speed (do not
 * kill them for trying)
 */
const double max_speed = 10.0;

/* objects should not be permitted to percieve more than max_perceive_range
 * or perceive less than min_percieve_range.
 * If they do, adjust their perceive range to the appropriate bound
 * (do not kill them for trying)
 */
const double max_perceive_range = 100.0;
const double min_perceive_range = 2.0;

const int grid_max = 500;
const int win_x_size = 500;
const int win_y_size = 500;

const double min_delta_time = 1.0e-6; // minimum time between scheduling an
                                // event and when that event can occur

/*
 * You may ignore the parameters after this line.  Just extra
 * stuff I added to the solution
 */

/*
 * set your encounter resolution strategy here.  Only affects the
 * case where both objects want to eat each other and both objects
 * "succeed"
 */
//const EncounterResolver encounter_strategy = FASTER_GUY_WINS;
//const EncounterResolver encounter_strategy = EVEN_MONEY;
const EncounterResolver encounter_strategy = BIG_GUY_WINS;

const SimulationTerminationStrategy termination_strategy =  
// RUN_TILL_ONE_SPECIES_LEFT;
// RUN_TILL_HALF_EXTINCT;
RUN_TILL_EVENTS_EXHAUSTED;      // probably runs forever, Algae Spores
