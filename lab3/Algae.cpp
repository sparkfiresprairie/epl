#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "Algae.h"
#include "Event.h"
#include "Params.h"
#include "tokens.h"
#include "Window.h"

using namespace std;
using String = std::string;

Initializer<Algae> __Algae_initializer;

void Algae::initialize(void)
{
    LifeForm::add_creator(Algae::create, "Algae");
}

String Algae::species_name(void) const
{
    return "Algae";
}

String Algae::player_name(void) const
{
    return "Algae";
}

Algae::Algae(void) {
    SmartPointer<Algae> self{ this };
    photo_event = new Event(algae_photo_time,
        [self](void) { self->photosynthesize(); });
}

void Algae::draw(int x, int y) const
{
#if DEBUG
    cout << "drawing Algae";
#endif /* DEBUG */
    win.draw_rectangle(x, y, x + 3, y + 3, true);
}

Color Algae::my_color(void) const
{
    return GREEN;
}

SmartPointer<LifeForm> Algae::create(void)
{
    return new Algae;
}

Action Algae::encounter(const ObjInfo&)
{
    return LIFEFORM_IGNORE;
}

void Algae::photosynthesize(void)
{
    photo_event = 0;
    if (!is_alive) { return; }
    energy += Algae_energy_gain;
    if (energy > 2.0 * start_energy) {
        SmartPointer<Algae> child = new Algae;
        reproduce(child);
    }
    SmartPointer<Algae> self{ this };
    photo_event = new Event(algae_photo_time,
        [self](void) { self->photosynthesize(); });
}

