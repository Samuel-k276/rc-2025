#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../common/constants.h"
#include "events.h"
#include "storage.h"

bool space_for_new_event() {
    int next_eid = find_next_available_eid();
    return next_eid <= 999;
}

int new_eid() {
    int next_eid = find_next_available_eid();
    if (next_eid > 999) {
        return -1;
    }
    return next_eid;
}

Event load_event_from_disk(int eid) {
    Event event;
    event.owner_uid = "";
    event.total_seats = 0;
    event.reserved_seats = 0;
    event.state = 1;

    if (!event_exists(eid)) {
        return event;
    }

    std::string start_info = load_event_start_info(eid);
    if (start_info.empty()) {
        return event;
    }

    std::string uid, name, desc_fname, date_time;
    int event_attend;
    if (parse_event_start_info(start_info, uid, name, desc_fname, event_attend, date_time)) {
        event.owner_uid = uid;
        event.name = name;
        event.file_name = desc_fname;
        event.date_time = date_time;
        event.total_seats = event_attend;
        event.reserved_seats = load_event_reserved_seats(eid);
        event.state = 1; // default state, will be recalculated when needed
    }

    return event;
}

std::string list_events() {
    std::string message = "RLS OK";

    std::vector<int> eids = get_all_event_ids();
    for (int eid : eids) {
        Event e = load_event_from_disk(eid);
        if (e.owner_uid.empty()) continue;

        int state = get_event_status(eid);
        message += " " + std::to_string(eid) + " " + e.name + " " + std::to_string(state) + " " + e.date_time;
    }

    message += "\n\n";
    return message;
}

std::string add_event(std::string uid, std::string name, std::string file_name, std::string date_time,
                      int total_seats) {
    int eid = new_eid();
    if (eid == -1) {
        return "RCE NOK\n";
    }

    create_event_directory(eid);

    std::string start_date = date_time.substr(0, 10);
    std::string start_time = date_time.substr(11, 5);

    save_event_start(eid, uid, name, file_name, total_seats, start_date, start_time);
    save_event_res_file(eid, 0);
    add_event_to_user_created(eid, uid);

    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << eid;
    std::string eid_str = oss.str();
    std::string message = "RCE OK " + eid_str;
    return message;
}

bool event_exist(int eid) { return event_exists(eid); }

bool uid_has_events(std::string uid) {
    std::vector<int> eids = get_user_created_events(uid);
    return !eids.empty();
}

std::string get_user_events(std::string uid) {
    std::vector<int> eids = get_user_created_events(uid);
    std::string message = "RME OK";
    for (int eid : eids) {
        int state = get_event_status(eid);
        message += " " + std::to_string(eid) + " " + std::to_string(state);
    }
    message += "\n";
    return message;
}

Event *get_event(int eid) {
    // Returns pointer to static variable - only valid until next call
    static Event cached_event;
    cached_event = load_event_from_disk(eid);
    return &cached_event;
}

int get_event_status(int eid) {
    if (!event_exists(eid)) {
        return -1;
    }

    if (event_end_exists(eid)) {
        return 3; // closed
    }

    Event e = load_event_from_disk(eid);
    if (e.owner_uid.empty()) {
        return -1;
    }

    int state = 1;
    int reserved_seats = load_event_reserved_seats(eid);
    if (e.total_seats > 0 && reserved_seats >= e.total_seats) {
        state = 2; // sold out
    }

    // Check if event date has passed and close it if needed
    std::string date_time = e.date_time;
    if (!date_time.empty() && date_time.length() >= 16) {
        try {
            // Parse dd-mm-yyyy hh:mm
            int day = std::stoi(date_time.substr(0, 2));
            int month = std::stoi(date_time.substr(3, 2));
            int year = std::stoi(date_time.substr(6, 4));
            int hour = std::stoi(date_time.substr(11, 2));
            int min = std::stoi(date_time.substr(14, 2));

            // Convert event date to time_t
            std::tm event_tm = {};
            event_tm.tm_mday = day;
            event_tm.tm_mon = month - 1;
            event_tm.tm_year = year - 1900;
            event_tm.tm_hour = hour;
            event_tm.tm_min = min;
            event_tm.tm_sec = 0;

            std::time_t event_time = std::mktime(&event_tm);
            std::time_t current_time = std::time(nullptr);

            if (event_time < current_time) {
                close_event(eid);
                return 0; // past
            }
        } catch (...) {
            // skip expiration check if date parsing fails
        }
    }

    return state;
}

bool owner_of_event(std::string uid, int eid) {
    Event e = load_event_from_disk(eid);
    return e.owner_uid == uid;
}

void close_event(int eid) {
    std::string end_datetime = get_current_datetime();
    save_event_end(eid, end_datetime);
}

bool enough_seats(int eid, int seats) {
    Event e = load_event_from_disk(eid);
    int reserved_seats = load_event_reserved_seats(eid);
    return reserved_seats + seats <= e.total_seats;
}

void add_reservation(std::string uid, int eid, int seats) {
    int reserved_seats = load_event_reserved_seats(eid);
    int new_reserved_seats = reserved_seats + seats;

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    char buf[20];
    std::strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", &tm);
    std::string ts(buf);

    // Persist reservation to disk
    save_reservation(eid, uid, seats, ts);
    save_event_res_file(eid, new_reserved_seats);
}

bool uid_has_reservations(std::string uid) {
    std::vector<std::tuple<int, std::string, int>> reservations = get_user_reservations_from_disk(uid);
    return !reservations.empty();
}

std::string get_user_reservations(std::string uid) {
    std::string message = "RMR OK";
    std::vector<std::tuple<int, std::string, int>> reservations = get_user_reservations_from_disk(uid);

    // Only return the last 50 reservations
    int start = std::max(0, (int)reservations.size() - 50);
    for (size_t i = start; i < reservations.size(); ++i) {
        int eid = std::get<0>(reservations[i]);
        std::string timestamp = std::get<1>(reservations[i]);
        int seats = std::get<2>(reservations[i]);

        // Split timestamp into date and time parts (format: "dd-mm-yyyy hh:mm:ss")
        size_t space_pos = timestamp.find(' ');
        std::string date_part = (space_pos != std::string::npos) ? timestamp.substr(0, space_pos) : timestamp;
        std::string time_part = (space_pos != std::string::npos) ? timestamp.substr(space_pos + 1) : "";

        message += " " + std::to_string(eid) + " " + date_part + " " + time_part + " " + std::to_string(seats);
    }
    message += "\n";
    return message;
}
