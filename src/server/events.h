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
int add_event(int uid); // TODO: Implement the whole event
void remove_event(int eid);
bool event_exist(int eid);
bool uid_has_events(int uid);
// TODO:
//std::vector<Event*> get_user_events(int uid);
#endif