#include "input.h"
#include <string>

const std::regex password_regex("[a-zA-Z0-9]{" + std::to_string(PASSWORD_LENGTH) + "}");
const std::regex uid_regex("[0-9]{" + std::to_string(UID_LENGTH) + "}");

bool is_password_valid(std::string password) {
    if (password.empty()) {
        return false;
    }

    return std::regex_match(password, password_regex);
}

bool is_uid_valid(std::string uid) {
    if (uid.empty()) {
        return false;
    }

    return std::regex_match(uid, uid_regex);
}
