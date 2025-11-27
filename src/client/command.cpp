#include "command.h"
#include "../common/input.h"
#include <cstring>
#include <iostream>
#include <sstream>

bool is_valid_command(std::string command) {
    return user_commands.find(command) != user_commands.end();
}

UserCommand get_command(std::string command) {
    return user_commands.at(command);
}


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
    message = command_to_string.at(LOGIN) + " " + uid + " " + password + "\n";
    return true;
}

// logout doesnt receive arguments
bool parse_logout_input(std::stringstream &args, std::string &message) {
    if (!args.eof()) {
        std::cerr << "Invalid logout arguments" << std::endl;
        return false;
    }
    message = command_to_string.at(LOGOUT) + "\n";
    return true;
}