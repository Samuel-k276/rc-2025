#include "input_handler.h"
#include "../common/constants.h"
#include "../common/input.h"
#include "session.h"
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

bool is_valid_client_command(std::string command) { return user_commands.find(command) != user_commands.end(); }

UserCommand get_command(std::string command) { return user_commands.at(command); }

bool parse_login_input(std::stringstream &args, std::string &message) {
    std::string uid, password;
    if (!(args >> uid >> password)) {
        std::cerr << "Invalid login arguments" << std::endl;
        return false;
    }
    if (!is_uid_valid(uid)) {
        std::cerr << "Invalid UID: " << uid << std::endl;
        return false;
    }
    if (!is_password_valid(password)) {
        std::cerr << "Invalid password: " << password << std::endl;
        return false;
    }
    set_temp_user_id(uid);
    set_temp_user_password(password);
    message = command_to_string.at(LOGIN) + " " + uid + " " + password + "\n";
    return true;
}

// logout doesnt receive arguments, it gets the user id and password from the session
bool parse_logout_input(std::stringstream &args, std::string &message) {
    std::string extra;
    if (args >> extra) {
        std::cerr << "Invalid logout arguments" << std::endl;
        return false;
    }
    std::string uid = get_user_id();
    std::string password = get_user_password();
    message = command_to_string.at(LOGOUT) + " " + uid + " " + password + "\n";
    return true;
}

// unregister doesnt receive arguments, it gets the user id and password from the session
bool parse_unregister_input(std::stringstream &args, std::string &message) {
    std::string extra;
    if (args >> extra) {
        std::cerr << "Invalid unregister arguments" << std::endl;
        return false;
    }
    std::string uid = get_user_id();
    std::string password = get_user_password();
    message = command_to_string.at(UNREGISTER) + " " + uid + " " + password + "\n";
    return true;
}

bool parse_myevents_input(std::stringstream &args, std::string &message) {
    std::string extra;
    if (args >> extra) {
        std::cerr << "Invalid myevents arguments" << std::endl;
        return false;
    }
    std::string uid = get_user_id();
    std::string password = get_user_password();
    message = command_to_string.at(MYEVENTS) + " " + uid + " " + password + "\n";
    return true;
}

// exit doesnt receive arguments, it just exits the program if the user is logged out
// exit\n
bool parse_exit_input(std::stringstream &args) {
    std::string extra;
    if (args >> extra) {
        std::cerr << "Invalid exit arguments" << std::endl;
        return false;
    }
    return true;
}

bool is_valid_num_attendees(std::string num_attendees) {
    const int num = std::stoi(num_attendees);
    return num >= 10 && num <= 999;
}

// get the file data and size from the file
// event_fname can contain a path, but Fname should only contain the filename
// Returns true if successful, false otherwise
bool get_file_data(std::string event_fname, std::string &Fdata, std::string &Fsize, std::string &Fname) {
    std::ifstream file(event_fname);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << event_fname << std::endl;
        return false;
    }

    // Extract only the filename (without path)
    std::filesystem::path path(event_fname);
    Fname = path.filename().string();

    // Check file size (maximum size is 10 MB = 10.106 bytes)
    auto file_size = std::filesystem::file_size(event_fname);
    if (file_size > MAX_FILE_SIZE) {
        std::cerr << "File size exceeds maximum allowed size (10 MB): " << file_size << " bytes" << std::endl;
        file.close();
        return false;
    }

    // Read file data
    Fdata.resize(file_size);
    file.read(Fdata.data(), file_size);
    Fsize = std::to_string(file_size);
    file.close();
    return true;
}

// CRE UID password name event_date attendance_size Fname Fsize Fdata
bool parse_create_event_input(std::stringstream &args, std::string &message) {
    std::string name, event_fname, date, time, num_attendees;
    if (!(args >> name >> event_fname >> date >> time >> num_attendees)) {
        std::cerr << "Invalid create event arguments" << std::endl;
        return false;
    }
    if (name.length() > 10) {
        std::cerr << "Name is too long: " << name << std::endl;
        return false;
    }
    std::string date_time = date + " " + time;
    if (!is_date_time_valid(date_time)) {
        std::cerr << "Invalid date/time: " << date << " " << time << std::endl;
        return false;
    }
    if (!is_valid_num_attendees(num_attendees)) {
        std::cerr << "Invalid number of attendees: " << num_attendees << std::endl;
        return false;
    }
    std::string uid = get_user_id();
    std::string password = get_user_password();
    std::string Fdata, Fsize, Fname;
    if (!get_file_data(event_fname, Fdata, Fsize, Fname)) {
        std::cerr << "Failed to read file: " << event_fname << std::endl;
        return false;
    }
    message = command_to_string.at(CREATE_EVENT) + " " + uid + " " + password + " " + name + " " + date + " " + time +
              " " + num_attendees + "\n";
    return true;
}

bool is_valid_eid(std::string eid) {
    return eid.length() == 3 && std::all_of(eid.begin(), eid.end(), ::isdigit) && std::stoi(eid) > 0;
}

bool parse_close_event_input(std::stringstream &args, std::string &message) {
    std::string event_id;
    if (!(args >> event_id)) {
        std::cerr << "Invalid close event arguments" << std::endl;
        return false;
    }
    if (!is_valid_eid(event_id)) {
        std::cerr << "Invalid event ID: " << event_id << std::endl;
        return false;
    }
    std::string uid = get_user_id();
    std::string password = get_user_password();
    message = command_to_string.at(CLOSE_EVENT) + " " + uid + " " + password + " " + event_id + "\n";
    return true;
}

bool parse_list_events_input(std::stringstream &args, std::string &message) {
    std::string extra;
    if (args >> extra) {
        std::cerr << "Invalid list events arguments" << std::endl;
        return false;
    }
    message = command_to_string.at(LIST_EVENTS) + "\n";
    return true;
}

bool parse_show_event_details_input(std::stringstream &args, std::string &message) {
    std::string event_id;
    if (!(args >> event_id)) {
        std::cerr << "Invalid show event details arguments" << std::endl;
        return false;
    }
    if (!is_valid_eid(event_id)) {
        std::cerr << "Invalid event ID: " << event_id << std::endl;
        return false;
    }
    message = command_to_string.at(SHOW_EVENT_DETAILS) + " " + event_id + "\n";
    return true;
}

bool parse_reserve_input(std::stringstream &args, std::string &message) {
    std::string event_id, num_seats;
    if (!(args >> event_id >> num_seats)) {
        std::cerr << "Invalid reserve arguments" << std::endl;
        return false;
    }
    if (!is_valid_eid(event_id)) {
        std::cerr << "Invalid event ID: " << event_id << std::endl;
        return false;
    }
    std::string uid = get_user_id();
    std::string password = get_user_password();
    message = command_to_string.at(RESERVE) + " " + uid + " " + password + " " + event_id + " " + num_seats + "\n";
    return true;
}

bool parse_change_pass_input(std::stringstream &args, std::string &message) {
    std::string old_password, new_password;
    if (!(args >> old_password >> new_password)) {
        std::cerr << "Invalid change pass arguments" << std::endl;
        return false;
    }
    if (!is_password_valid(old_password)) {
        std::cerr << "Invalid old password: " << old_password << std::endl;
        return false;
    }
    if (!is_password_valid(new_password)) {
        std::cerr << "Invalid new password: " << new_password << std::endl;
        return false;
    }
    std::string uid = get_user_id();
    set_temp_user_id(uid);
    set_temp_user_password(new_password);
    message = command_to_string.at(CHANGE_PASS) + " " + uid + " " + old_password + " " + new_password + "\n";
    return true;
}
