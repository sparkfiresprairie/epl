//
//  xw3893.h
//  Project3
//
//  Created by Xingyuan Wang on 5/8/15.
//  Copyright (c) 2015 Engineering Programming Language. All rights reserved.
//
#if !(_xw3893_h)
#define _xw3893_h 1

#include "LifeForm.h"
#include "Init.h"

class xw3893 : public LifeForm {
protected:
    static constexpr double health_threshold = 0.5;
    static constexpr int algae_count_threshold = 4;
    static constexpr int enemy_count_threshold = 2;
    static constexpr int fellow_count_threshold = 4;
    static constexpr int max_perceive_count = 3;
    int algae_count;
    int fellow_count;
    int enemy_count;
    int course_changed;
    static void initialize(void);
    void spawn(void);
    void hunt(void);
    void live(void);
    Event* hunt_event;
    int my_ID;
    std::string my_name;
    std::string enemy_name;
    std::string fav_food;
    std::string get_nth_token(std::string s, uint64_t n, std::string delimiter) const;
    bool is_same_species(const std::string& suspect) const;
public:
    xw3893(void);
    ~xw3893(void);
    Color my_color(void) const;   // defines LifeForm::my_color
    static LifeForm* create(void);
    std::string species_name(void) const;
    Action encounter(const ObjInfo&);
    std::string player_name(void) const { return "xw3893"; }
    friend class Initializer<xw3893>;
};


#endif /* !(_xw3893_h) */
