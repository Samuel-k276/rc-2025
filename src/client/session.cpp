#include "session.h"

#include <string>

UserSession user_session;

std::string get_user_id() { return user_session.uid; }

void set_user_id(std::string uid) { user_session.uid = uid; }

std::string get_user_password() { return user_session.password; }

void set_user_password(std::string password) { user_session.password = password; }

bool is_user_logged_in() { return user_session.uid.empty() || user_session.password.empty(); }