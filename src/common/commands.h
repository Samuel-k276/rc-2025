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

bool is_valid_command_udp(std::string command);
bool is_valid_command_tcp(std::string command);
bool is_valid_command(std::string command);
CommandType get_command_type(std::string command);
#endif