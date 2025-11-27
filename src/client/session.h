#ifndef SESSION_H
#define SESSION_H

#include <string>

struct UserSession {
    std::string uid;
    std::string password;
};

/**
 * Get the user ID.
 * @return std::string: user ID
 */
std::string get_user_id();

/**
 * Set the user ID.
 * @param uid: user ID
 */
void set_user_id(std::string uid);

/**
 * Get the user password.
 * @return std::string: user password
 */
std::string get_user_password();

/**
 * Set the user password.
 * @param password: user password
 */
void set_user_password(std::string password);
#endif