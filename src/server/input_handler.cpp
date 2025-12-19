#include "input_handler.h"
#include "../common/commands.h"
#include "../common/constants.h"
#include "../common/input.h"
#include <iostream>
#include <sstream>
#include <string>

const int space_and_newline_length = 3;

// Parse LIN UID password - login command (UDP)
// Format: LIN UID password\n
bool parse_login_command(std::string message, std::string &uid, std::string &password) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    ss >> command >> uid >> password;

    if (get_command_type(command) != LOGIN) {
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

    return true;
}

// Parse LOU UID password - logout command (UDP)
// Format: LOU UID password\n
bool parse_logout_command(std::string message, std::string &uid, std::string &password) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        if (message.length() == 6) {
            return false;
        }
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    ss >> command >> uid >> password;

    if (get_command_type(command) != LOGOUT) {
        return false;
    }

    if (!is_uid_valid(uid)) {
        return false;
    }

    if (!is_password_valid(password)) {
        return false;
    }

    return true;
}

// Parse UNR UID password - unregister command (UDP)
// Format: UNR UID password\n
bool parse_unregister_command(std::string message, std::string &uid, std::string &password) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        if (message.length() == 6) {
            return false;
        }
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    ss >> command >> uid >> password;

    if (get_command_type(command) != UNREGISTER) {
        return false;
    }

    if (!is_uid_valid(uid)) {
        return false;
    }

    if (!is_password_valid(password)) {
        return false;
    }

    return true;
}

// Parse LME UID password - list my events command (UDP)
// Format: LME UID password\n
bool parse_list_my_events_command(std::string message, std::string &uid, std::string &password) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        if (message.length() == 6) {
            return false;
        }
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    ss >> command >> uid >> password;

    if (get_command_type(command) != LIST_MY_EVENTS) {
        return false;
    }

    if (!is_uid_valid(uid)) {
        return false;
    }

    if (!is_password_valid(password)) {
        return false;
    }

    return true;
}

// Parse LMR UID password - list my reservations command (UDP)
// Format: LMR UID password\n
bool parse_list_my_reservations_command(std::string message, std::string &uid, std::string &password) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    ss >> command >> uid >> password;

    if (get_command_type(command) != LIST_MY_RESERVATIONS) {
        return false;
    }

    if (!is_uid_valid(uid)) {
        return false;
    }

    if (!is_password_valid(password)) {
        return false;
    }

    return true;
}

// Parse CRE UID password name event_date attendance_size Fname Fsize Fdata - create event command (TCP)
// Format: CRE UID password name event_date attendance_size Fname Fsize Fdata\n
// event_date format: dd-mm-yyyy hh:mm
bool parse_create_command(std::string message, std::string &uid, std::string &password, std::string &name,
                          std::string &event_date, std::string &attendance_size, std::string &fname, std::string &fsize,
                          std::string &fdata) {
    if (message.empty()) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string date, time;
    ss >> command >> uid >> password >> name >> date >> time >> attendance_size >> fname >> fsize;
    
    event_date = date + " " + time;

    if (get_command_type(command) != CREATE_EVENT) {
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
    if (!is_name_valid(name)) {
        std::cerr << "Invalid event name: " << name << std::endl;
        return false;
    }
    if (!is_date_time_valid(event_date)) {
        std::cerr << "Invalid date/time: " << event_date << std::endl;
        return false;
    }
    if (!is_attendance_size_valid(attendance_size)) {
        std::cerr << "Invalid attendance size: " << attendance_size << std::endl;
        return false;
    }
    if (!is_fname_valid(fname)) {
        std::cerr << "Invalid file name: " << fname << std::endl;
        return false;
    }
    if (!is_fsize_valid(fsize)) {
        std::cerr << "Invalid file size: " << fsize << std::endl;
        return false;
    }

    // Extract file data from message - it comes after fname and fsize
    std::string fname_fsize_pattern = fname + " " + fsize + " ";
    size_t pattern_pos = message.find(fname_fsize_pattern);
    if (pattern_pos != std::string::npos) {
        size_t fdata_start = pattern_pos + fname_fsize_pattern.length();
        size_t fdata_end = message.find('\n', fdata_start);
        if (fdata_end == std::string::npos) {
            fdata_end = message.length();
        }
        fdata = message.substr(fdata_start, fdata_end - fdata_start);
    } else {
        // Fallback if pattern matching fails
        size_t fname_pos = message.find(fname);
        if (fname_pos != std::string::npos) {
            size_t search_start = fname_pos + fname.length();
            size_t fsize_pos = message.find(" " + fsize + " ", search_start);
            if (fsize_pos != std::string::npos) {
                size_t fdata_start = fsize_pos + fsize.length() + 2;
                size_t fdata_end = message.find('\n', fdata_start);
                if (fdata_end == std::string::npos) {
                    fdata_end = message.length();
                }
                fdata = message.substr(fdata_start, fdata_end - fdata_start);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

// Parse CLS UID password EID - close event command (TCP)
// Format: CLS UID password EID\n
bool parse_close_command(std::string message, std::string &uid, std::string &password, std::string &eid) {
    if (message.empty()) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    ss >> command >> uid >> password >> eid;

    if (get_command_type(command) != CLOSE_EVENT) {
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
    if (!is_eid_valid(eid)) {
        std::cerr << "Invalid event ID: " << eid << std::endl;
        return false;
    }

    return true;
}

// Parse LST - list events command (TCP)
// Format: LST\n
bool parse_list_command(std::string message) {
    std::stringstream ss(message);
    std::string command;
    std::string extra;
    if (!(ss >> command)) {
        return false;
    }

    if (get_command_type(command) != LIST_EVENTS) {
        return false;
    }

    if (ss >> extra) {
        std::cerr << "Invalid list, no extra arguments" << std::endl;
        return false;
    }

    return true;
}

// Parse SED EID - show event details command (TCP)
// Format: SED EID\n
bool parse_show_event_details_command(std::string message, std::string &eid) {
    if (message.empty()) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    ss >> command >> eid;

    if (get_command_type(command) != SHOW_EVENT_DETAILS) {
        return false;
    }

    if (!is_eid_valid(eid)) {
        std::cerr << "Invalid event ID: " << eid << std::endl;
        return false;
    }

    return true;
}

// Parse RID UID password EID people - reserve command (TCP)
// Format: RID UID password EID people\n
bool parse_reserve_command(std::string message, std::string &uid, std::string &password, std::string &eid,
                           std::string &number_of_people) {
    if (message.empty()) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    ss >> command >> uid >> password >> eid >> number_of_people;

    if (get_command_type(command) != RESERVE) {
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
    if (!is_eid_valid(eid)) {
        std::cerr << "Invalid event ID: " << eid << std::endl;
        return false;
    }

    return true;
}

// Parse CPS UID oldPassword newPassword - change password command (TCP)
// Format: CPS UID oldPassword newPassword\n
bool parse_change_pass_command(std::string message, std::string &uid, std::string &old_password,
                               std::string &new_password) {
    if (message.empty()) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;

    if (!(ss >> command >> uid >> old_password >> new_password)) {
        return false;
    }

    if (get_command_type(command) != CHANGE_PASS) {
        return false;
    }

    if (!is_uid_valid(uid)) {
        std::cerr << "Invalid UID: " << uid << std::endl;
        return false;
    }
    if (!is_password_valid(old_password)) {
        std::cerr << "Invalid old password" << std::endl;
        return false;
    }
    if (!is_password_valid(new_password)) {
        std::cerr << "Invalid new password" << std::endl;
        return false;
    }

    return true;
}
