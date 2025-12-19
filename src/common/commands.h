#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <unordered_map>

enum CommandType {
    // UDP commands
    LOGIN,
    LOGOUT,
    UNREGISTER,
    LIST_MY_EVENTS,
    LIST_MY_RESERVATIONS,
    // TCP commands
    CREATE_EVENT,
    CLOSE_EVENT,
    LIST_EVENTS,
    SHOW_EVENT_DETAILS,
    RESERVE,
    CHANGE_PASS,
    // Invalid command
    INVALID_COMMAND,
};

const static std::unordered_map<std::string, CommandType> CommandUDPMap = {
    {"LIN", LOGIN}, {"LOU", LOGOUT}, {"UNR", UNREGISTER}, {"LME", LIST_MY_EVENTS}, {"LMR", LIST_MY_RESERVATIONS},
};

const static std::unordered_map<std::string, CommandType> CommandTCPMap = {
    {"CRE", CREATE_EVENT},       {"CLS", CLOSE_EVENT}, {"LST", LIST_EVENTS},
    {"SED", SHOW_EVENT_DETAILS}, {"RID", RESERVE},     {"CPS", CHANGE_PASS},
};

/**
 * Check if a UDP command is valid.
 * @param command: command
 * @return bool: true if the command is valid, false otherwise
 */
bool is_valid_command_udp(std::string command);

/**
 * Check if a TCP command is valid.
 * @param command: command
 * @return bool: true if the command is valid, false otherwise
 */
bool is_valid_command_tcp(std::string command);

/**
 * Check if a command is valid.
 * @param command: command
 * @return bool: true if the command is valid, false otherwise
 */
bool is_valid_command(std::string command);

/**
 * Get the command type from a command.
 * @param command: command
 * @return CommandType: command type
 */
CommandType get_command_type(std::string command);

/**
 * Check if a login command is valid.
 * @param message: message
 * @return bool: true if the login command is valid, false otherwise
 */
bool is_valid_login_command(std::string message);

/**
 * Check if a logout command is valid.
 * @param message: message
 * @return bool: true if the logout command is valid, false otherwise
 */
bool is_valid_logout_command(std::string message);

/**
 * Check if a unregister command is valid.
 * @param message: message
 * @return bool: true if the unregister command is valid, false otherwise
 */
bool is_valid_unregister_command(std::string message);

/**
 * Check if a list my events command is valid.
 * @param message: message
 * @return bool: true if the list my events command is valid, false otherwise
 */
bool is_valid_list_my_events_command(std::string message);

/**
 * Check if a list my reservations command is valid.
 * @param message: message
 * @return bool: true if the list my reservations command is valid, false otherwise
 */
bool is_valid_list_my_reservations_command(std::string message);

bool is_valid_create_command(std::string message);

bool is_valid_close_command(std::string message);

bool is_valid_change_pass_command(std::string message);
#endif