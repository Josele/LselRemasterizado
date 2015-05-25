#include "observer.h"


void observer_init(observer_t* this, observer_notify_func_t notify) {
	this->notify = notify;
}

void observer_notify(observer_t* this) {
	this->notify(this);
}

void observable_init(observable_t* this) {
	this->nobservers = 0;
	this->nevents = 0;
}

void observable_register_observer(observable_t* this, observer_t* obs) {
	this->observer[this->nobservers++] = obs;
}

void observable_notify_observers(observable_t* this) {
	int i;
	this->nevents++;
	for (i = 0; i < this->nobservers; ++i) {
		observer_notify(this->observer[i]);
	}
}

int observable_get_nobservers(observable_t* this) {
	return this->nobservers;
}

int observable_get_nevents(observable_t* this) {
	return this->nevents;
}
