#include <iostream>
#include <unordered_map>

#include "user.h"

std::unordered_map<std::string, User> users;

/**
 * Add a user to the system, if the user is already registered, it will be overwritten.
 * User is logged in by default.
 * @param uid: user ID
 * @param password: user password
 */
void add_user(std::string uid, std::string password) {
    users[uid] = User{uid, password, true};
}

/**
 * Remove a user from the system
 * @param uid: user ID
 */
void remove_user(std::string uid) {
    users.erase(uid);
}

/**
 * Get a user from the system, if the user is not registered, it will create an empty user.
 * @param uid: user ID
 * @return User&: reference to user
 */
User& get_user(std::string uid) {
    return users[uid];
}

/**
 * Check if a user is registered
 * @param uid: user ID
 * @return bool: true if the user is registered, false otherwise
 */
bool is_user_registered(std::string uid) {
    return users.find(uid) != users.end();
}

/**
 * Check if a user is logged in
 * @param uid: user ID
 * @return bool: true if the user is logged in, false otherwise
 */
bool is_user_logged_in(std::string uid) {
    return get_user(uid).is_logged_in;
}

/**
 * Login a user
 * @param uid: user ID
 */
void login_user(std::string uid) {
    get_user(uid).is_logged_in = true;
}

/**
 * Logout a user
 * @param uid: user ID
 */
void logout_user(std::string uid) {
    get_user(uid).is_logged_in = false;
}