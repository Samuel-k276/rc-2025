#ifndef USER_H
#define USER_H

#include <cstdbool>
#include <string>

struct User {
    std::string uid;
    std::string password;
    bool is_logged_in;
};

void add_user(std::string uid, std::string password);
void remove_user(std::string uid);
User &get_user(std::string uid);
bool is_user_registered(std::string uid);
bool is_user_logged_in(std::string uid);
void login_user(std::string uid);
void logout_user(std::string uid);

#endif