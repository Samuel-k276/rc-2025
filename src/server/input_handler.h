#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "../common/commands.h"
#include <string>

// Parse and validate server command messages
bool parse_login_command(std::string message, std::string &uid, std::string &password);
bool parse_logout_command(std::string message, std::string &uid, std::string &password);
bool parse_unregister_command(std::string message, std::string &uid, std::string &password);
bool parse_list_my_events_command(std::string message, std::string &uid, std::string &password);
bool parse_list_my_reservations_command(std::string message, std::string &uid, std::string &password);
bool parse_create_command(std::string message, std::string &uid, std::string &password, std::string &name,
                          std::string &event_date, std::string &attendance_size, std::string &fname, std::string &fsize,
                          std::string &fdata);
bool parse_close_command(std::string message, std::string &uid, std::string &password, std::string &eid);
bool parse_list_command(std::string message);
bool parse_show_event_details_command(std::string message, std::string &eid);
bool parse_reserve_command(std::string message, std::string &uid, std::string &password, std::string &eid,
                           std::string &number_of_people);
bool parse_change_pass_command(std::string message, std::string &uid, std::string &old_password,
                               std::string &new_password);

#endif
