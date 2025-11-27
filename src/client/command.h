#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <unordered_map>

enum UserCommand {
    // Authentication & Account Management
    LOGIN,
    LOGOUT,
    UNREGISTER,
    CHANGE_PASS,
    // User Events & Reservations
    MYEVENTS,
    MYRESERVATIONS,
    // Event Management
    CREATE_EVENT,
    CLOSE,
    LIST,
    SHOW,
    // Reservations
    RESERVE,
    // System
    EXIT,
};

// From user input to command
const std::unordered_map<std::string, UserCommand> user_commands = {
    // Authentication & Account Management
    {"login", LOGIN},
    {"logout", LOGOUT},
    {"unregister", UNREGISTER},
    {"changePass", CHANGE_PASS},
    // User Events & Reservations
    {"myevents", MYEVENTS},
    {"mye", MYEVENTS},
    {"myreservations", MYRESERVATIONS},
    {"myr", MYRESERVATIONS},
    // Event Management
    {"create", CREATE_EVENT},
    {"close", CLOSE},
    {"list", LIST},
    {"show", SHOW},
    // Reservations
    {"reserve", RESERVE},
    // System
    {"exit", EXIT},
};

// From command enum to server command string
const std::unordered_map<UserCommand, std::string> command_to_string = {
    // Authentication & Account Management
    {LOGIN, "LIN"},
    {LOGOUT, "LOU"},
    {UNREGISTER, "UNR"},
    {CHANGE_PASS, "CPS"},
    // User Events & Reservations
    {MYEVENTS, "LME"},
    {MYRESERVATIONS, "LMR"},
    // Event Management
    {CREATE_EVENT, "CRE"},
    {CLOSE, "CLS"},
    {LIST, "LST"},
    {SHOW, "SED"},
    // Reservations
    {RESERVE, "RID"},
    // System
    {EXIT, "EXT"},
};

/**
 * Check if a command is valid.
 * @param command: command
 * @return bool: true if the command is valid, false otherwise
 */
bool is_valid_client_command(std::string command);

/**
 * Get the command from a command.
 * @param command: command
 * @return UserCommand: command
 */
UserCommand get_command(std::string command);

/**
 * Parse the login input and return the message in the format "LIN <uid> <password>".
 * @param args: arguments
 * @param message: message
 * @return bool: true if the login input is parsed successfully, false otherwise
 */
bool parse_login_input(std::stringstream &args, std::string &message);

/**
 * Parse the logout input and return the message in the format "LOU <uid> <password>".
 * @param args: arguments
 * @param message: message
 * @return bool: true if the logout input is parsed successfully, false otherwise
 */
bool parse_logout_input(std::stringstream &args, std::string &message);

/**
 * Parse the change pass input and return the message in the format "CPS <uid> <old_password> <new_password>".
 * @param args: arguments
 * @param message: message
 * @return bool: true if the change pass input is parsed successfully, false otherwise
 */
bool parse_change_pass_input(std::stringstream &args, std::string &message);

/**
 * Parse the unregister input and return the message in the format "UNR <uid> <password>".
 * @param args: arguments
 * @param message: message
 * @return bool: true if the unregister input is parsed successfully, false otherwise
 */
bool parse_unregister_input(std::stringstream &args, std::string &message);

/**
 * Parse the exit input and return the message in the format "EXT".
 * @param args: arguments
 * @return bool: true if the exit input is parsed successfully, false otherwise
 */
bool parse_exit_input(std::stringstream &args);

#endif