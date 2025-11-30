#include <iostream>
#include <unordered_map>
#include <set>

#include "events.h"
#include "../common/constants.h"

std::unordered_map<int, Event> events;
std::set<int> used_eids;

int new_eid() {
    for (int i = 1; i < MAX_EIDS; i++) {
        if(used_eids.find(i) == used_eids.end()) {
            used_eids.insert(i);
            return i;
        }
    }
    return -1;
}
int add_event() { // TODO: Implement the whole event input
    int eid = new_eid();
    if (eid == -1) {
        return -1;
    }
    Event event;
    event.eid = eid;

    events[eid] = event; 
    return eid;
} 

void remove_event(int eid) {
    events.erase(eid);
    used_eids.erase(eid);
}
/*
Event &get_event(int eid) {
    //TODO

}

std::vector<Event*> get_user_events(int uid) {
    //TODO
}*/