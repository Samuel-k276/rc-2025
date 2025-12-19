#ifndef EVENTS_H
#define EVENTS_H

#include <cstdbool>
#include <string>
#include <vector>
#include <set>


struct Reservation {
    int eid;
    std::string timestamp;
    int seats;
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


bool space_for_new_event();
int new_eid();
std::string add_event(std::string uid, std::string name, std::string file_name, std::string date_time, int total_seats);
bool event_exist(int eid);
bool uid_has_events(std::string uid);
std::string get_user_events(std::string uid);
Event* get_event(int eid);
int get_event_status(int eid);
bool owner_of_event(std::string uid, int eid);
void close_event(int eid);

bool enough_seats(Event event, int seats);
void add_reservation(std::string uid, int eid, int seats);
bool uid_has_reservations(std::string uid);
std::string get_user_reservations(std::string uid);
#endif