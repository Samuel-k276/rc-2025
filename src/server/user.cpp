#include <iostream>

#include "storage.h"
#include "user.h"

/**
 * Add a user to the system, if the user is already registered, it will be overwritten.
 * User is logged in by default.
 * @param uid: user ID
 * @param password: user password
 */
void add_user(std::string uid, std::string password) {
    save_user_password(uid, password);
    create_user_login_file(uid);
}

/**
 * Remove a user from the system
 * @param uid: user ID
 */
void remove_user(std::string uid) {
    remove_user_password_file(uid);
    remove_user_login_file(uid);
}

/**
 * Get a user from the system (loads from disk)
 * @param uid: user ID
 * @return User: user struct loaded from disk
 */
User get_user(std::string uid) {
    User user;
    user.uid = uid;
    if (user_password_exists(uid)) {
        user.password = load_user_password(uid);
        user.is_logged_in = user_login_exists(uid);
    }
    return user;
}

/**
 * Check if a user is registered
 * @param uid: user ID
 * @return bool: true if the user is registered, false otherwise
 */
bool is_user_registered(std::string uid) { return user_password_exists(uid); }

/**
 * Check if a user is logged in
 * @param uid: user ID
 * @return bool: true if the user is logged in, false otherwise
 */
bool is_user_logged_in(std::string uid) { return user_login_exists(uid); }

/**
 * Login a user
 * @param uid: user ID
 */
void login_user(std::string uid) { create_user_login_file(uid); }

/**
 * Logout a user
 * @param uid: user ID
 */
void logout_user(std::string uid) { remove_user_login_file(uid); }

/**
 * Change a user's password
 * @param uid: user ID
 * @param new_password: new password
 */
void change_user_password(std::string uid, std::string new_password) { save_user_password(uid, new_password); }