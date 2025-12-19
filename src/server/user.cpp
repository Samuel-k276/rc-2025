#include <iostream>

#include "storage.h"
#include "user.h"

// Register a new user (or overwrite existing) and log them in
void add_user(std::string uid, std::string password) {
    save_user_password(uid, password);
    create_user_login_file(uid);
}

// Remove user account and all associated files
void remove_user(std::string uid) {
    remove_user_password_file(uid);
    remove_user_login_file(uid);
}

// Load user data from disk
User get_user(std::string uid) {
    User user;
    user.uid = uid;
    if (user_password_exists(uid)) {
        user.password = load_user_password(uid);
        user.is_logged_in = user_login_exists(uid);
    }
    return user;
}

bool is_user_registered(std::string uid) {
    return user_password_exists(uid);
}

bool is_user_logged_in(std::string uid) {
    return user_login_exists(uid);
}

void login_user(std::string uid) {
    create_user_login_file(uid);
}

void logout_user(std::string uid) {
    remove_user_login_file(uid);
}

void change_user_password(std::string uid, std::string new_password) {
    save_user_password(uid, new_password);
}
