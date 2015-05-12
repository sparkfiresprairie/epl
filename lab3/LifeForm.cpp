/* main test simulator */
#include <iostream>
#include "CraigUtils.h"
#include "Window.h"
#include "tokens.h"
#include "ObjInfo.h"
#include "QuadTree.h" 
#include "Params.h"
#include "LifeForm.h"
#include "Event.h"

using namespace std;

template <typename T>
void bound(T& x, const T& min, const T& max) {
	assert(min < max);
	if (x > max) { x = max; }
	if (x < min) { x = min; }
}



ObjInfo LifeForm::info_about_them(SmartPointer<LifeForm> neighbor) {
	ObjInfo info;

	info.species = neighbor->species_name();
	info.health = neighbor->health();
	info.distance = pos.distance(neighbor->position());
	info.bearing = pos.bearing(neighbor->position());
	info.their_speed = neighbor->speed;
	info.their_course = neighbor->course;
	return info;
}

/**
 *  the event handler function for the border cross event
 */
void LifeForm::border_cross(void) {
    if (!is_alive) return;
    border_cross_event = nullptr;
    update_position();
    check_encounter();
    compute_next_move();
}

/**
 *  the callback function for region resizes (invoked by the quadtree)
 */
void LifeForm::region_resize(void) {
    update_position();
    compute_next_move();
}

void LifeForm::eat(SmartPointer<LifeForm> that) {
    that->die();
    energy -= eat_cost_function(this->energy, that->energy);
    if (energy < min_energy) {
        die();
        return;
    }
    double e = that->energy * eat_efficiency;
    SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
    (void) new Event (digestion_time, [self, e](void){ self->gain_energy(e); });
}

void LifeForm::gain_energy(double e) {
    // this lifeform may die in digestion time
    if (!is_alive) return;
    energy += e;
    if (energy < min_energy) {
        energy = 0;
        die();
    }
}

/**
 *  subtract age_penalty from energy
 */
void LifeForm::age(void) {
    if (!is_alive) return;
    energy -= age_penalty;
    if (energy > min_energy) {
        SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
        (void) new Event(age_frequency, [self](void){ self->age(); });
    }
    else {
        energy = 0;
        die();
    }
}

/**
 *  calculate the current position for an object.
 */
void LifeForm::update_position(void) {
    double delta_time = Event::now() - this->update_time;
    Point newpos;
    // don't update position if time less than min_delta_time
    if (!is_alive || delta_time < min_delta_time) return;
    
    // calculate new position
    newpos.xpos = pos.xpos + cos(course) * delta_time * speed;
    newpos.ypos = pos.ypos + sin(course) * delta_time * speed;
    
    // go out of bound, die
    if (space.is_out_of_bounds(newpos)) {
        energy = 0;
        die();
        return;
    }
    
    // stationary, nothing happens
    if (newpos == pos) return;
    
    // lack of energy, die
    energy -= movement_cost(speed, delta_time);
    if (energy < min_energy) {
        energy = 0;
        die();
        return;
    }
    
    update_time = Event::now();
    space.update_position(pos, newpos);
    pos = newpos;
}


/**
 *  check to see if there's another object
 *  within encounter_distance.  If there's
 *  an object nearby, invoke resove_encounter
 *  on ourself with the closest object
 */
void LifeForm::check_encounter(void) {
    if (!is_alive) return;
    auto closest_obj = space.closest(pos);
    update_position();
    if( is_alive && closest_obj->is_alive && pos.distance(closest_obj->pos) < encounter_distance )
        resolve_encounter(closest_obj);
}

void LifeForm::resolve_encounter(SmartPointer<LifeForm> that) {
    energy -= encounter_penalty;
    that->energy -= encounter_penalty;
    if (energy < min_energy) {
        energy = 0;
        die();
    }
    if (that->energy < min_energy) {
        energy = 0;
        die();
    }
    
    if (!is_alive || !(that->is_alive)) return;
    
    auto this_info = that->info_about_them(SmartPointer<LifeForm>(this));
    auto that_info = this->info_about_them(that);
    auto this_act = this->encounter(that_info);
    auto that_act = that->encounter(this_info);
    
    if (this_act == LIFEFORM_EAT && that_act == LIFEFORM_EAT) {
        auto rand1 = drand48();
        auto rand2 = drand48();
        if ( rand1 < eat_success_chance(energy, that->energy) && rand2 < eat_success_chance(that->energy, energy)) {
            switch (encounter_strategy) {
                case EVEN_MONEY:
                    if (drand48() < 0.5) { eat(that); }
                    else { that->eat(SmartPointer<LifeForm>(this)); }
                    break;
                case BIG_GUY_WINS:
                    if (energy > that->energy) { eat(that); }
                    else { that->eat(SmartPointer<LifeForm>(this)); }
                    break;
                case UNDERDOG_IS_HERE:
                    if (energy <= that->energy) { eat(that); }
                    else { that->eat(SmartPointer<LifeForm>(this)); }
                    break;
                case FASTER_GUY_WINS:
                    if (speed > that->speed) { eat(that); }
                    else { that->eat(SmartPointer<LifeForm>(this)); }
                    break;
                case SLOWER_GUY_WINS:
                    if (energy <= that->energy) { eat(that); }
                    else { that->eat(SmartPointer<LifeForm>(this)); }
                    break;
                default:
                    break;
            }
        }
        else if ( rand1 < eat_success_chance(energy, that->energy) && rand2 >= eat_success_chance(that->energy, energy) ) {
            eat(that);
        }
        else if ( rand1 >= eat_success_chance(energy, that->energy) && rand2 < eat_success_chance(that->energy, energy)) {
            that->eat(SmartPointer<LifeForm>(this));
        }
        else {}
        
    }
    else if ( this_act == LIFEFORM_EAT && that_act == LIFEFORM_IGNORE ) {
        if (drand48() < eat_success_chance(energy, that->energy)) {
            eat(that);
        }
    }
    else if ( this_act == LIFEFORM_IGNORE && that_act == LIFEFORM_EAT ) {
        if (drand48() < eat_success_chance(that->energy, energy)) {
            that->eat(SmartPointer<LifeForm>(this));
        }
    }
    else {}
}

/**
 *  a simple function that creates the next border_cross_event
 */
void LifeForm::compute_next_move(void) {
    // cancel previous border_cross_event
    if (border_cross_event != nullptr) {
        border_cross_event->cancel();
        border_cross_event = nullptr;
    }
    
    // schedule a new new border_cross event
    SmartPointer<LifeForm> self = SmartPointer<LifeForm>(this);
    if (speed > 0.0) {
        double delta_time = (space.distance_to_edge(pos, course) + Point::tolerance)/speed;
        border_cross_event = new Event(delta_time, [self](void){ self->border_cross(); });
    }
}

void LifeForm::set_course(double c) {
    if (!is_alive) return;
    update_position();
    course = c;
    compute_next_move();
}
void LifeForm::set_speed(double s) {
    if (!is_alive) return;
    update_position();
    speed = s < max_speed? s : max_speed;
    compute_next_move();
}

void LifeForm::reproduce(SmartPointer<LifeForm> child) {
    if (!is_alive) return;
    
    update_position();
    
    if (Event::now() - reproduce_time < min_reproduce_time) return;
    
    energy = child->energy = energy * (1 - reproduce_cost) / 2;
    
    if (energy < min_energy) {
        child->energy = 0;
        child->is_alive = false;
        energy = 0;
        die();
    }
    else {
        SmartPointer<LifeForm> nearest;
        // try 5 times, if all fail, ignore the attempt to reproduce
        for (int i = 0 ; i < 5; ++i) {
            // place child in [encouter_distance, reproduce_dist] from parent
            // of courese child shoude be in bound
            do {
                double r = encounter_distance + drand48() * (reproduce_dist - encounter_distance);
                double rad = drand48() * 2 * M_PI;
                child->pos.xpos = pos.xpos + r * cos(rad);
                child->pos.ypos = pos.ypos + r * sin(rad);
            } while (space.is_out_of_bounds(child->pos));
            
            // if child's position is within other lifeform's encounter_distance
            // fail this try
            nearest = space.closest(child->pos);
            if (nearest) {
                nearest->update_position();
                if (nearest->pos.distance(child->pos) > encounter_distance) break;
            }
            else break;
        }
        
        child->start_point = child->pos;
        space.insert(child, child->pos, [child](void) { child->region_resize(); });
        (void) new Event(age_frequency, [child](void) { child->age(); });
        child->is_alive = true;
        reproduce_time = Event::now();
    }
}

ObjList LifeForm::perceive(double perceive_range) {
    if (!is_alive) return ObjList(0);
    
    if (perceive_range > max_perceive_range) { perceive_range = max_perceive_range; }
    else if (perceive_range < min_perceive_range) { perceive_range = min_perceive_range; }
    
    energy -= perceive_cost(perceive_range);
    if (energy < min_energy) {
        energy = 0;
        die();
        return ObjList(0);
    }
    
    vector<SmartPointer<LifeForm>> obj_vector = space.nearby(pos, perceive_range);
    ObjList obj_info_vector(0);
    for (auto obj : obj_vector) {
        obj->update_position();
        if (pos.distance(obj->pos) < perceive_range) {
            obj_info_vector.push_back(info_about_them(obj));
        }
    }
    return obj_info_vector;
}


