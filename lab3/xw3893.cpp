//
//  xw3893.cpp
//  Project3
//
//  Created by Xingyuan Wang on 5/8/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "CraigUtils.h"
#include "Params.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Random.h"
#include "xw3893.h"
#include "Window.h"

#if defined (_MSC_VER)
using namespace epl;
#endif
using namespace std;
using String = std::string;

Initializer<xw3893> __xw3893_initializer;

//int xw3893::total_num = 0 ; // sorry, no statics allowed

String xw3893::species_name(void) const
{
    if (enemy_name == "I am safe.") {
        return my_name + ":"
            + to_string(my_ID) + ":"
            + to_string(algae_count) + ":"
            + to_string(fellow_count) + ":"
            + to_string(enemy_count);
    }
    else return enemy_name;
}

Action xw3893::encounter(const ObjInfo& info)
{
    if (is_same_species(info.species)) {
        /* don't be cannibalistic */
        set_course(info.bearing + M_PI);
        return LIFEFORM_IGNORE;
    }
    else {
        if (info.species == fav_food && health() > health_threshold) enemy_name = "I am safe.";
        else enemy_name = info.species;
        hunt_event->cancel();
        SmartPointer<xw3893> me{this};
        hunt_event = new Event(0.0, [me] (void) { me->hunt(); });
        return LIFEFORM_EAT;
    }
}

string xw3893::get_nth_token(string s, uint64_t n, string delimiter) const {
    uint64_t pos = 0;
    vector<string> token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    token.push_back(s);
    if (n <= token.size()) {
        return token[n-1];
    }
    else return string("");
}

bool xw3893::is_same_species(const string& suspect) const {
    // avoid camouflage: using unique ID
    return get_nth_token(suspect, 1, ":") == my_name && (get_nth_token(suspect, 2, ":")) != to_string(my_ID);
}

void xw3893::initialize(void)
{
	LifeForm::add_creator(xw3893::create, "xw3893");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
xw3893::xw3893()
{
    my_ID = rand();
    my_name = "xw3893";
    enemy_name = "I am safe.";
    fav_food = "Algae";
    algae_count = 0;
    fellow_count = 0;
    enemy_count = 0;
    hunt_event = Nil<Event>();
    SmartPointer<xw3893> me{this};
    (void) new Event(0.0, [me] (void) { me->live();});
}


xw3893::~xw3893()
{
}

void xw3893::spawn(void)
{
	xw3893* child = new xw3893;
	reproduce(child);
}


Color xw3893::my_color(void) const
{
  	return MAGENTA ;
}  

LifeForm* xw3893::create(void)
{
	xw3893* res = Nil<xw3893>();
	res = new xw3893;
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
void xw3893::live(void)
{
	set_course(drand48() * 2.0 * M_PI);
	set_speed(max_speed);
	SmartPointer<xw3893> me{this};
	hunt_event = new Event(5.0, [me] (void) { me->hunt();});
}

void xw3893::hunt(void)
{
    ObjList prey;
    double distance = HUGE;
    double bearing = 0;
    bool proceed = false;
    
    
    hunt_event = Nil<Event>();
    if (health() == 0) { return; }
    
    for (int i = 0 ; i < max_perceive_count ; ++i) {
        double max_gain = 0;
        double perceive_range = min_perceive_range + (max_perceive_range - min_perceive_range) * (i + 1) /max_perceive_count;
        prey = perceive(perceive_range);
        algae_count = 0;
        fellow_count = 0;
        enemy_count = 0;
        enemy_name = "I am safe.";
        
        for (auto obj : prey) {
            if (obj.species == fav_food) {
                double gain = obj.health;
                if (gain > max_gain) {
                    distance = obj.distance;
                    max_gain = gain;
                    bearing  = obj.bearing;
                }
                ++algae_count;
            }
            else if (is_same_species(obj.species)) ++fellow_count;
            else ++enemy_count;
        }
        
        if (algae_count > 0) {
            set_course(bearing);
            set_speed(min(distance/min_delta_time, max_speed));
            proceed = true;
            break;
        }
    }
    
    // fail to find any fellow creatures, find enemy
    if (!proceed) {
        double max_health_ratio = -HUGE;
        for (auto obj : prey) {
            if (!is_same_species(obj.species)) {
                double health_ratio = health()/obj.health;
                if (health_ratio > max_health_ratio) {
                    enemy_name       = obj.species;
                    distance         = obj.distance;
                    max_health_ratio = health_ratio;
                    bearing          = obj.bearing;
                }
            }
        }
        if (max_health_ratio != -HUGE) {
            set_course(bearing);
            set_speed(drand48()*max_speed);
            proceed = true;
        }
    }
    
    // fail to find any algae, then search for fellow creatures
    // whose surrounding has the most algaes
    if (!proceed) {
        double max_algae_number = -HUGE;
        for (auto obj : prey) {
            if (is_same_species(obj.species)) {
                int algae_number = stoi(get_nth_token(obj.species, 3, ":"));
                if (algae_number > max_algae_number) {
                    distance         = obj.distance;
                    max_algae_number  = algae_number;
                    bearing          = obj.bearing;
                }
            }
        }
        if (max_algae_number > 3) {
            set_course(bearing);
            set_speed(drand48()*max_speed);
            proceed = true;
        }
    }
    
    if (!proceed)
    {
        distance = grid_max/max_speed;
        set_course(get_course() + M_PI + (drand48() - 0.5) * M_PI/4);
        set_speed(drand48()*max_speed*0.1);
        proceed = true;
    }
    
    
    
    SmartPointer<xw3893> self{this};
    hunt_event = new Event(min(distance/get_speed(), 5.0), [self] (void) {self->hunt();});
    if (health() > 5 && algae_count > algae_count_threshold && enemy_count <= enemy_count_threshold) spawn();
    else if (health() > 100 && enemy_count > enemy_count_threshold && fellow_count <= fellow_count_threshold) spawn();
}

