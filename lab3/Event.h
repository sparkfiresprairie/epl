#if !(_Event_h)
#define _Event_h 1

#include <cassert>
#include <functional>
#include <limits.h>

#include "Params.h"
#include "SimTime.h"            // for the SimTime class

/* necessary forward reference */
class PQueue;

/*
 * Class name: Event
 * Class characterization: Abstract base class
 *                         virtual functions: operator()
 * Description:
 *  An event is something that happens at a specific time
 *  Events can be compared (comparison is defined over the time at
 *  which they occur).  < and == are defined.
 *  and events can be applied (via () ).
 *  There are no arguments allowed when applying an event.
 *  There is no return value (or the return value is ignored
 *
 * Recommended Usage: Simulation
 *  Derive templatized classes from Event.
 *  make a priority queue (e.g. heap) that contains pointers to
 *  Events (instances of the derived classes).
 *
 * Implementation:
 *  We rely on a class "SimTime" to exist.  Most probably SimTime is a typedef
 *  to either int or double.
 *  If SimTime does not support operator =, then you must comment out the
 *  operator = for class Event.
 *
 */
class Event {
private:
    SimTime t;
    using Handler = std::function<void(void)>;
    Handler doit;
    static PQueue equeue;         // a priority queue of all events
    static SimTime _now;
    bool in_queue;

    /* Implementation NOTE:
       If you inline these, you need to include the definition of PQueue
       in every file that includes Event.h... Since PQueue is based
       on templates, this is very expensive... (compiling is slow)
       so, I choose not to inline them */
    void insert(void);            // insert this event into the priority queue
    void remove(void);            // remove this event from the priority queue
    bool active;

public:
    /* interface */
    void operator()(void) { if (active) { doit(); } }

    static SimTime now(void) { return _now; }
    static unsigned num_events(void); // the total number of events in the world
    static void do_next(void);    // process the next event


  /* constructors and destructors */
    Event(SimTime delta_time, Handler f) : doit(f) {
        if (delta_time < min_delta_time) delta_time = min_delta_time;
        t = _now + delta_time;
        active = true;
        insert();
    }
    ~Event(void);

    void cancel(void) { if (this) active = false; }
    bool is_active(void) const { return this && active; }

private:
    /* assignment and copying are forbidden in Events */
    Event(const Event& e) = delete;
    void operator=(const Event&) = delete;

    /* The EventCompare class is used in Event.cc to implement the Event Queue */
    friend struct EventCompare;
};

#endif /* !(_Event_h) */
