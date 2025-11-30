#ifndef EVENTS_H
#define EVENTS_H

#include <cstdbool>
#include <string>
#include <vector>

struct Reservation {
    int uid;
    int seats;
    std::string timestamp;
};

struct Event {
    int eid;
    std::string name;
    std::string date_time;
    int total_seats;
    int reserved_seats;
    int owner_uid;
    int state;

    std::string file_name;
    std::vector<Reservation> reservations;
};

int new_eid();
void add_event(int eid); // TODO: Implement the whole event
void remove_event(int eid);
//Event &get_event(int eid);
//std::vector<Event*> get_user_events(int eid);

#endif