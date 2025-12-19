#include <iostream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <set>
#include <chrono>
#include <ctime>

#include "events.h"
#include "../common/constants.h"

std::vector<Event> events(1000);
int next_eid = 1;
std::unordered_map<std::string, std::set<int>> events_by_uid;
std::unordered_map<std::string, std::vector<Reservation>> reservations_by_uid;

bool space_for_new_event() {
    return next_eid < 999;
}

int new_eid() {
    if(next_eid > 999) {
        return -1;
    }
    return next_eid++;
}

std::string add_event(std::string uid, std::string name, std::string file_name, std::string date_time, int total_seats) {
    int eid = new_eid();

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

    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << eid;
    std::string eid_str = oss.str();
    std::string message = "RCE OK " + eid_str;
    return message;
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

Event* get_event(int eid) {
    return &events[eid];
}

bool enough_seats(Event event, int seats) {
    return event.reserved_seats + seats > event.total_seats;
}

void add_reservation(std::string uid, int eid, int seats) {
    Event* event = get_event(eid);
    event->reserved_seats += seats;

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::string ts = std::ctime(&t);
    ts.pop_back();

    reservations_by_uid[uid].push_back({eid, ts, seats});
    return;
}

bool uid_has_reservations(std::string uid) {
    if(reservations_by_uid.find(uid) == reservations_by_uid.end()) {
        return false;
    }
    return true;
}

std::string get_user_reservations(std::string uid) {
    std::string message = "RMR OK";
    std::vector<Reservation> reservations = reservations_by_uid[uid];
    for(Reservation res : reservations) {
        message += " " + std::to_string(res.eid) + " " + res.timestamp + " " + std::to_string(res.seats);
    }
    message += "\n";
    return message;
}
