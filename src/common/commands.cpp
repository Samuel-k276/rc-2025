#include "commands.h"

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