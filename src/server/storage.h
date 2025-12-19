#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include <tuple>

// Initialize storage directories (USERS and EVENTS)
void init_storage();

// Load all data from disk into memory
void load_data_from_disk();

// User persistence functions
void save_user_password(const std::string& uid, const std::string& password);
void create_user_login_file(const std::string& uid);
void remove_user_login_file(const std::string& uid);
void remove_user_password_file(const std::string& uid);
bool user_password_exists(const std::string& uid);
bool user_login_exists(const std::string& uid);
std::string load_user_password(const std::string& uid);

// Event persistence functions
void create_event_directory(int eid);
void save_event_start(int eid, const std::string& uid, const std::string& name, 
                      const std::string& desc_fname, int event_attend, 
                      const std::string& start_date, const std::string& start_time);
void save_event_res_file(int eid, int reserved_seats);
void save_event_description_file(int eid, const std::string& fname, const std::string& fdata);
void save_event_end(int eid, const std::string& end_datetime);
void add_event_to_user_created(int eid, const std::string& uid);
bool event_end_exists(int eid);
int load_event_reserved_seats(int eid);
std::string load_event_start_info(int eid);
bool event_exists(int eid);
int find_next_available_eid();
std::vector<int> get_all_event_ids();
bool parse_event_start_info(const std::string& info, std::string& uid, std::string& name, 
                           std::string& desc_fname, int& event_attend, std::string& date_time);
std::vector<int> get_user_created_events(const std::string& uid);
std::vector<std::tuple<int, std::string, int>> get_user_reservations_from_disk(const std::string& uid);

// Reservation persistence functions
void save_reservation(int eid, const std::string& uid, int res_num, const std::string& res_datetime);
void add_reservation_to_user_reserved(const std::string& uid, int eid, int res_num, const std::string& res_datetime);

// Utility functions
std::string get_current_datetime();
std::string format_eid(int eid);
bool check_and_close_expired_event(int eid);

#endif

