#ifndef INPUT_H
#define INPUT_H

#include <cstdbool>
#include <regex>
#include <string>

#include "constants.h"

/**
 * Check if a password is valid.
 * A valid password is a string of PASSWORD_LENGTH alphanumeric characters.
 * @param password: password
 * @return bool: true if the password is valid, false otherwise
 */
bool is_password_valid(std::string password);

/**
 * Check if a UID is valid.
 * A valid UID is a string of UID_LENGTH digits.
 * @param uid: UID
 * @return bool: true if the UID is valid, false otherwise
 */
bool is_uid_valid(std::string uid);

/**
 * Check if a EID is valid.
 * A valid EID is a string of EID_LENGTH digits.
 * @param eid: EID
 * @return bool: true if the EID is valid, false otherwise
 */
bool is_eid_valid(std::string eid);
#endif