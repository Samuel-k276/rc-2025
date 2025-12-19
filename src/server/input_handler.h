#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <string>
#include "../common/commands.h"

// UDP command parsers
/**
 * Parse and validate login command: "LIN <uid> <password>"
 * @param message: message to parse
 * @param uid: output parameter for UID
 * @param password: output parameter for password
 * @return bool: true if valid, false otherwise
 */
bool parse_login_command(std::string message, std::string &uid, std::string &password);

/**
 * Parse and validate logout command: "LOU <uid> <password>"
 * @param message: message to parse
 * @param uid: output parameter for UID
 * @param password: output parameter for password
 * @return bool: true if valid, false otherwise
 */
bool parse_logout_command(std::string message, std::string &uid, std::string &password);

/**
 * Parse and validate unregister command: "UNR <uid> <password>"
 * @param message: message to parse
 * @param uid: output parameter for UID
 * @param password: output parameter for password
 * @return bool: true if valid, false otherwise
 */
bool parse_unregister_command(std::string message, std::string &uid, std::string &password);

/**
 * Parse and validate list my events command: "LME <uid> <password>"
 * @param message: message to parse
 * @param uid: output parameter for UID
 * @param password: output parameter for password
 * @return bool: true if valid, false otherwise
 */
bool parse_list_my_events_command(std::string message, std::string &uid, std::string &password);

/**
 * Parse and validate list my reservations command: "LMR <uid> <password>"
 * @param message: message to parse
 * @param uid: output parameter for UID
 * @param password: output parameter for password
 * @return bool: true if valid, false otherwise
 */
bool parse_list_my_reservations_command(std::string message, std::string &uid, std::string &password);

// TCP command parsers
/**
 * Parse and validate create event command: "CRE <uid> <password> <name> <event_date> <attendance_size> <fname> <fsize> <fdata>"
 * @param message: message to parse
 * @param uid: output parameter for UID
 * @param password: output parameter for password
 * @param name: output parameter for event name
 * @param event_date: output parameter for event date
 * @param attendance_size: output parameter for attendance size
 * @param fname: output parameter for file name
 * @param fsize: output parameter for file size
 * @param fdata: output parameter for file data
 * @return bool: true if valid, false otherwise
 */
bool parse_create_command(std::string message, std::string &uid, std::string &password,
                          std::string &name, std::string &event_date, std::string &attendance_size,
                          std::string &fname, std::string &fsize, std::string &fdata);

/**
 * Parse and validate close event command: "CLS <uid> <password> <eid>"
 * @param message: message to parse
 * @param uid: output parameter for UID
 * @param password: output parameter for password
 * @param eid: output parameter for event ID
 * @return bool: true if valid, false otherwise
 */
bool parse_close_command(std::string message, std::string &uid, std::string &password, std::string &eid);

/**
 * Parse and validate list events command: "LST"
 * @param message: message to parse
 * @return bool: true if valid, false otherwise
 */
bool parse_list_command(std::string message);

/**
 * Parse and validate reserve command: "RID <uid> <password> <eid> <number_of_people>"
 * @param message: message to parse
 * @param uid: output parameter for UID
 * @param password: output parameter for password
 * @param eid: output parameter for event ID
 * @param number_of_people: output parameter for number of people
 * @return bool: true if valid, false otherwise
 */
bool parse_reserve_command(std::string message, std::string &uid, std::string &password,
                           std::string &eid, std::string &number_of_people);

/**
 * Parse and validate change password command: "CPS <uid> <old_password> <new_password>"
 * @param message: message to parse
 * @param uid: output parameter for UID
 * @param old_password: output parameter for old password
 * @param new_password: output parameter for new password
 * @return bool: true if valid, false otherwise
 */
bool parse_change_pass_command(std::string message, std::string &uid, std::string &old_password, std::string &new_password);

#endif

