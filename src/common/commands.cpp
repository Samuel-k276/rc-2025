#include "commands.h"
#include "input.h"
#include <cstdbool>
#include <iostream>
#include <sstream>
#include <string>

#include "../common/constants.h"

const int space_and_newline_length = 3;

CommandType get_command_type(std::string command) {
    if (CommandUDPMap.find(command) != CommandUDPMap.end()) {
        return CommandUDPMap.at(command);
    } else if (CommandTCPMap.find(command) != CommandTCPMap.end()) {
        return CommandTCPMap.at(command);
    }
    return INVALID_COMMAND;
}

bool is_valid_command_udp(std::string command) { return CommandUDPMap.find(command) != CommandUDPMap.end(); }

bool is_valid_command_tcp(std::string command) { return CommandTCPMap.find(command) != CommandTCPMap.end(); }

bool is_valid_command(std::string command) { return is_valid_command_udp(command) || is_valid_command_tcp(command); }

bool is_valid_login_command(std::string message) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

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

bool is_valid_logout_command(std::string message) {    
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        if (message.length() == 6) {
            return true;
        }
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

    return get_command_type(command) == LOGOUT && is_uid_valid(uid) && is_password_valid(password);
}

bool is_valid_unregister_command(std::string message) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        if (message.length() == 6) {
            return true;
        }
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

    return get_command_type(command) == UNREGISTER && is_uid_valid(uid) && is_password_valid(password);
}

bool is_valid_list_my_events_command(std::string message) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        if (message.length() == 6) {
            return true;
        }
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

    return get_command_type(command) == LIST_MY_EVENTS && is_uid_valid(uid) && is_password_valid(password);
}

bool is_valid_list_my_reservations_command(std::string message) {
    if (message.empty() || message.length() != CMD_LENGTH + UID_LENGTH + PASSWORD_LENGTH + space_and_newline_length) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    ss >> command >> uid >> password;

    return get_command_type(command) == LIST_MY_RESERVATIONS && is_uid_valid(uid) && is_password_valid(password);
}

bool is_valid_create_command(std::string message) {
    if (message.empty()) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    std::string name;
    std::string event_date;
    std::string attendance_size;
    std::string fname;
    std::string fsize;
    std::string fdata;
    ss >> command >> uid >> password >> name >> event_date >> attendance_size >> fname >> fsize >> fdata;

    if (!is_uid_valid(uid)) {
        std::cerr << "Invalid UID: " << uid << std::endl;
        return false;
    }
    if (!is_password_valid(password)) {
        std::cerr << "Invalid password: " << password << std::endl;
        return false;
    }
    if (!is_name_valid(fname)) {
        std::cerr << "Invalid event name: " << name << std::endl;
        return false;
    }
    if (!is_date_time_valid(event_date)) {
        std::cerr << "Invalid password: " << password << std::endl;
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

    return true;
}

bool is_valid_close_command(std::string message) {
    if (message.empty()) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string password;
    std::string eid;

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


bool is_valid_change_pass_command(std::string message) {
    if (message.empty()) {
        return false;
    }

    std::stringstream ss(message);
    std::string command;
    std::string uid;
    std::string old_password;
    std::string new_password;
    
    if (!(ss >> command >> uid >> old_password >> new_password)) {
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

bool is_valid_list_command(std::string message) {
    std::stringstream ss(message);
    std::string command;
    std::string extra;
    if (!(ss >> command)) {
        return false;
    }

    if (ss >> extra) {
        std::cerr << "Invalid list, no extra arguments" << std::endl;
        return false;
    }

    return true;
}