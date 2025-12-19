#ifndef INPUT_H
#define INPUT_H

#include <cstdbool>
#include <regex>
#include <string>

#include "constants.h"

// Input validation functions
bool is_password_valid(std::string password);
bool is_uid_valid(std::string uid);
bool is_eid_valid(std::string eid);

bool is_name_valid(std::string name);
bool is_date_time_valid(std::string date_time);
bool is_attendance_size_valid(std::string attendance_size);
bool is_fname_valid(std::string fname);
bool is_fsize_valid(std::string fsize);
bool is_number_of_people_valid(std::string number_of_people);
#endif