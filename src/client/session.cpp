#include "session.h"

#include <string>

UserSession user_session;

UserSession temp_user_session;

std::string get_user_id() { return user_session.uid; }

void set_user_id(std::string uid) { user_session.uid = uid; }

std::string get_user_password() { return user_session.password; }

void set_user_password(std::string password) { user_session.password = password; }

bool is_user_logged_in() { return !user_session.uid.empty() && !user_session.password.empty(); }

void set_temp_user_id(std::string uid) { temp_user_session.uid = uid; }

void set_temp_user_password(std::string password) { temp_user_session.password = password; }

void promote_temp_user_to_user() {
    user_session.uid = temp_user_session.uid;
    user_session.password = temp_user_session.password;
}

void clear_temp_user_session() { temp_user_session = UserSession{}; }

void clear_user_session() {
    user_session = UserSession{};
    temp_user_session = UserSession{};
}
