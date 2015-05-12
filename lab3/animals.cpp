#include <iostream>
#include <thread>
#include "LifeForm.h"
#include "Algae.h"
#include "Event.h"
#include "Params.h"
#include "Random.h"

namespace epl {
    std::default_random_engine random_generator;
    std::function<double(void)> drand48 = std::bind(std::uniform_real_distribution<double>{}, random_generator);
}

using namespace std;
using namespace epl;
const double Point::tolerance = 1.0e-6;

bool LifeForm::testMode = false;
void LifeForm::runTests(void) {}

/* The Tick class creates an event every 1.00 time units
* The event is used to add new Algae to the simulation and can
* also be used to add debugging hooks if you need them
*/
class Tick {
public:
    static void tock(void) {
        std::function<void(void)> callme = [](void) { tock(); };
#if ALGAE_SPORES    
        Algae::create_spontaneously();
#endif /* ALGAE_SPORES */
        if (Event::num_events() > 1)
            (void) new Event(1, callme);
    }
};

void delay(void) {
    std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
    new Event(1, &delay);
}

int main(int argc, char** argv) {
    double last_time = 0.0;
    double time_lapse;

    if (argc > 1)
        time_lapse = atof(argv[1]);
    else
        time_lapse = 1.0;

    LifeForm::create_life();
    new Event(1, &delay);
    Tick::tock();
    while (Event::num_events() > 0) {
        Event::do_next();
        // periodically redisplay everything
        if (Event::now() - last_time > time_lapse) {
            last_time = Event::now();
            LifeForm::redisplay_all();
        }
    }

    cerr << "Simulation Complete, hit ^C to terminate program\n";
    //  sleep(1000);
}
