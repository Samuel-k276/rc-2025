#include <iostream>
#include <unordered_map>
#include <set>

#include "events.h"
#include "../common/constants.h"

std::vector<Event> events(1000);
int next_eid = 1;
std::unordered_map<std::string, std::set<int>> events_by_uid;

int new_eid() {
    if(next_eid > 999) {
        return -1;
    }
    return next_eid++;
}

int add_event(std::string uid, std::string name, std::string file_name, std::string date_time, int total_seats) {
    int eid = new_eid();
    if (eid == -1) {
        return -1;
    }

    Event event;
    event.owner_uid = uid;
    event.name = name;
    event.file_name = file_name;
    event.date_time= date_time;
    event.total_seats = total_seats;
    event.reserved_seats = 0;
    event.state = 1;

    events[eid] = event;
    events_by_uid[uid].insert(eid);

    return eid;
} 

bool event_exist(int eid) {
    return 1 <= eid && eid < next_eid;
}
bool uid_has_events(std::string uid) {
    return events_by_uid.find(uid) != events_by_uid.end();
}

std::string get_user_events(std::string uid) {
    std::set<int> eids = events_by_uid[uid];
    std::string message = "RME OK";
    for (int eid : eids) {
        Event e = events[eid];
        message += " " + std::to_string(eid) + " " + std::to_string(e.state);
    }
    message += "\n";
    return message;
}