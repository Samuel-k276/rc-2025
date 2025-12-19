#include "input.h"
#include <string>

const std::regex password_regex("[a-zA-Z0-9]{" + std::to_string(PASSWORD_LENGTH) + "}");
const std::regex uid_regex("[0-9]{" + std::to_string(UID_LENGTH) + "}");
const std::regex eid_regex((std::string("^(?!0{") + std::to_string(EID_LENGTH) + "})[0-9]{" + std::to_string(EID_LENGTH) + "}$"));
const std::regex name_regex("^[A-Za-z0-9]{1,10}$");
const std::regex datetime_regex(
        "^(0[1-9]|[12][0-9]|3[01])-"
        "(0[1-9]|1[0-2])-"
        "[0-9]{4} "
        "([01][0-9]|2[0-3]):"
        "([0-5][0-9])$"
    );
const std::regex fname_regex("^[A-Za-z0-9_.-]{1,20}\\.[A-Za-z]{3}$");


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

bool is_eid_valid(std::string eid) {
    if (eid.empty()) {
        return false;
    }

    return std::regex_match(eid, eid_regex);
}

bool is_name_valid(std::string name) {
    if (name.empty()) {
        return false;
    }

    return std::regex_match(name, name_regex);
}

bool is_date_time_valid(std::string date_time) {
    if (date_time.empty()) {
        return false;
    }

    return std::regex_match(date_time, datetime_regex);
}

bool is_attendance_size_valid(std::string attendance_size) {
    if (!std::regex_match(attendance_size, std::regex("^[0-9]{2,3}$")))
        return false;

    int val = std::stoi(attendance_size);
    return val >= 10 && val <= 999;
}

bool is_fname_valid(std::string fname) {
    if (fname.empty()) {
        return false;
    }

    return std::regex_match(fname, fname_regex);
}

bool is_fsize_valid(std::string fsize) {
    if (!std::regex_match(fsize, std::regex("^[0-9]+$")))
        return false;

    long size = std::stol(fsize);
    return size >= 0 && size <= 10'000'000;
}