#include <iostream>
#include <cassert>
#include <algorithm>
#include <queue>
#include <vector>
#include "Params.h"
#include "Event.h"

using namespace std;

SimTime Event::_now = 0;

struct EventCompare {
  bool operator()(const Event* ep1, const Event* ep2) {
    return ep1->t > ep2->t;     // backwards on purpose so that events 
                                // are processed in increasing order
  }
};

class PQueue {
	vector<Event*> V;
public:
	PQueue(void) {} // normal construction
	~PQueue(void);
  
	void insert(Event* e) {
		V.push_back(e);
		push_heap(V.begin(), V.end(), EventCompare());
	}

	Event* pop_greatest(void) {
		pop_heap(V.begin(), V.end(), EventCompare());
		Event* e = V.back();
		V.pop_back();
		return e;
	}

  
  /*
   * remove an event from the queue
   */
  void remove(Event* e) {
	  e->cancel();
  }

  unsigned size(void) const {
    return V.end() - V.begin();
  }
};

/* delete all of the events */
PQueue::~PQueue() {
  /* this look relies on the events removing themselves from the queue */
  /* it's slow, but it's simple (and who cares how fast it is, 
     the program's over by now */
  while (!V.empty()) {
    Event* e = V.front();
    delete e;
  }
}

PQueue Event::equeue;


Event::~Event() {
	assert(!in_queue);
}

/*
 * simulate until there are no more events to simulate
 */
void Event::do_next(void) {
	Event* e = equeue.pop_greatest();
	e->in_queue = false;
	assert(e->t >= _now);
	_now = e->t;
#if DEBUG
	cout << "doing event at time " << _now << endl;
#endif /* DEBUG */
	(*e)();
	delete e;
}

unsigned Event::num_events(void) {
	return equeue.size();
}

void Event::remove(void) {
	assert(in_queue);
	equeue.remove(this);
	in_queue = 0;
}

void Event::insert() {
	in_queue = true;
	assert(Event::_now <= t);
	equeue.insert(this);
}


/*void Event::delete_matching(void* p)
{ 
  equeue.delete_matching(p);
}*/
