#include <iostream>
#include <unordered_map>
#include <set>

#include "events.h"
#include "../common/constants.h"

std::unordered_map<int, Event> events;
std::set<int> used_eids;
std::unordered_map<int, std::set<int>> events_by_uid;

int new_eid() {
    for (int i = 1; i < MAX_EIDS; i++) {
        if(used_eids.find(i) == used_eids.end()) {
            used_eids.insert(i);
            return i;
        }
    }
    return -1;
}

int add_event(int uid) { // TODO: Implement the whole event input
    int eid = new_eid();
    if (eid == -1) {
        return -1;
    }

    Event event;
    event.eid = eid;
    event.owner_uid = uid;
    // rest of input

    events[eid] = event;
    events_by_uid[uid].insert(eid);

    return eid;
} 

void remove_event(int eid) {
    auto it = events.find(eid);
    if (it == events.end()) return;

    int uid = it -> second.owner_uid;

    events.erase(eid);
    used_eids.erase(eid);

    auto& user_events = events_by_uid[uid];
    user_events.erase(eid);
    if(user_events.empty()) {
        events_by_uid.erase(uid);
    }
}

bool event_exist(int eid) {
    return events.find(eid) != events.end();
}
bool uid_has_events(int uid) {
    return events_by_uid.find(uid) != events_by_uid.end();
}

/*
Event &get_event(int eid) {
    //TODO

}

std::vector<Event*> get_user_events(int uid) {
    //TODO
}*/