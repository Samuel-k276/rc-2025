#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

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
    CLOSE_EVENT,
    LIST_EVENTS,
    SHOW_EVENT_DETAILS,
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
    {"close", CLOSE_EVENT},
    {"list", LIST_EVENTS},
    {"show", SHOW_EVENT_DETAILS},
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
    {CLOSE_EVENT, "CLS"},
    {LIST_EVENTS, "LST"},
    {SHOW_EVENT_DETAILS, "SED"},
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
 * Parse the myevnts input and return the message in the format "MEE <uid> <password>".
 * @param args: arguments
 * @param message: message
 * @return bool: true if the unregister input is parsed successfully, false otherwise
 */
bool parse_myevents_input(std::stringstream &args, std::string &message);
/**
 * Parse the exit input and return the message in the format "EXT".
 * @param args: arguments
 * @return bool: true if the exit input is parsed successfully, false otherwise
 */
bool parse_exit_input(std::stringstream &args);

/**
 * Parse the create event input and return the message.
 * @param args: arguments
 * @param message: message
 * @return bool: true if the create event input is parsed successfully, false otherwise
 */
bool parse_create_event_input(std::stringstream &args, std::string &message);

/**
 * Parse the close event input and return the message.
 * @param args: arguments
 * @param message: message
 * @return bool: true if the close event input is parsed successfully, false otherwise
 */
bool parse_close_event_input(std::stringstream &args, std::string &message);

/**
 * Parse the list events input and return the message.
 * @param args: arguments
 * @param message: message
 * @return bool: true if the list events input is parsed successfully, false otherwise
 */
bool parse_list_events_input(std::stringstream &args, std::string &message);

/**
 * Parse the show event details input and return the message.
 * @param args: arguments
 * @param message: message
 * @return bool: true if the show event details input is parsed successfully, false otherwise
 */
bool parse_show_event_details_input(std::stringstream &args, std::string &message);

/**
 * Parse the reserve input and return the message.
 * @param args: arguments
 * @param message: message
 * @return bool: true if the reserve input is parsed successfully, false otherwise
 */
bool parse_reserve_input(std::stringstream &args, std::string &message);

#endif

