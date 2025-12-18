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

/**
 * Check if the user is logged in.
 * @return bool: true if the user is logged in, false otherwise
 */
bool is_user_logged_in();

/**
 * Set the temporary user ID.
 * @param uid: temporary user ID
 */
void set_temp_user_id(std::string uid);

/**
 * Set the temporary user password.
 * @param password: temporary user password
 */
void set_temp_user_password(std::string password);

/**
 * Promote the temporary user to the user.
 */
void promote_temp_user_to_user();

/**
 * Clear the temporary user session.
 */
void clear_temp_user_session();

/**
 * Clear the user session.
 */
void clear_user_session();

#endif