#ifndef EVENTS_H
#define EVENTS_H

#include <cstdbool>
#include <string>
#include <vector>
#include <set>


struct Reservation {
    std::string eid;
    std::string uid;
    int seats;
    int timestamp;
};

struct Event {
    std::string owner_uid;
    std::string name;
    std::string file_name;
    std::string date_time;
    int total_seats;
    int reserved_seats;
    int state;
};

int new_eid();
int add_event(std::string uid);
bool event_exist(int eid);
bool uid_has_events(std::string uid);
std::string get_user_events(std::string uid);
#endif