#include "command.h"
#include "../common/input.h"
#include "session.h"
#include <cstring>
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
        /// FIXME
    set_user_id(uid);
    set_user_password(password);
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
    message = command_to_string.at(CHANGE_PASS) + " " + uid + " " + old_password + " " + new_password + "\n";
    return true;
}

// unregister doesnt receive arguments, it gets the user id and password from the session
bool parse_unregister_input(std::stringstream &args, std::string &message) {
    std::string extra;
    if (args >> extra) {
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
        return false;
    }
    std::string uid = get_user_id();
    std::string password = get_user_password();
    message = command_to_string.at(MYEVENTS) + " " + uid + " " + password + "\n";
    return true;
}

// exit doesnt receive arguments, it just exits the program if the user is logged out
bool parse_exit_input(std::stringstream &args) {
    if (!args.eof()) {
        std::cerr << "Invalid exit arguments" << std::endl;
        return false;
    }
    return true;
}