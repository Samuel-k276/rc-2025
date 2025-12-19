#include "date.h"

static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const int days_in_month_leap[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

bool is_leap_year(int year) { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); }

bool is_valid_day(int day, int month, int year) {
    if (day < 1 || day > (is_leap_year(year) ? days_in_month_leap[month - 1] : days_in_month[month - 1])) {
        return false;
    }
    return true;
}

bool is_valid_month(int month) { return month >= 1 && month <= 12; }

bool is_number(char number) { return number >= '0' && number <= '9'; }

// dd-mm-yyyy
bool is_valid_date(std::string date) {
    if (date.length() != 10) {
        return false;
    }
    for (size_t i = 0; i < date.length(); i++) {
        if (i == 2 || i == 5) {
            if (date[i] != '-') {
                return false;
            }
        } else {
            if (!is_number(date[i])) {
                return false;
            }
        }
    }
    int day = std::stoi(date.substr(0, 2));
    int month = std::stoi(date.substr(3, 2));
    int year = std::stoi(date.substr(6, 4));
    if (!is_valid_month(month) || !is_valid_day(day, month, year)) {
        return false;
    }
    return true;
}

// hh:mm
bool is_valid_time(std::string time) {
    if (time.length() != 5) {
        return false;
    }
    if (time[2] != ':') {
        return false;
    }
    for (size_t i = 0; i < time.length(); i++) {
        if (i == 2) {
            if (time[i] != ':') {
                return false;
            }
        } else {
            if (!is_number(time[i])) {
                return false;
            }
        }
    }
    int hour = std::stoi(time.substr(0, 2));
    int minute = std::stoi(time.substr(3, 2));
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return false;
    }
    return true;
}