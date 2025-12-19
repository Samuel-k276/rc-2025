#include "storage.h"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>
#include <vector>

static void create_directory_recursive(const std::string &path) {
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return;
    }

    size_t pos = path.find_last_of('/');
    if (pos != std::string::npos) {
        std::string parent = path.substr(0, pos);
        create_directory_recursive(parent);
    }

    mkdir(path.c_str(), 0700);
}

void init_storage() {
    create_directory_recursive("src/server/storage/USERS");
    create_directory_recursive("src/server/storage/EVENTS");
}

std::string get_current_datetime() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    char buf[20];
    std::strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", &tm);
    return std::string(buf);
}

std::string format_eid(int eid) {
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << eid;
    return oss.str();
}

void save_user_password(const std::string &uid, const std::string &password) {
    std::string user_dir = "src/server/storage/USERS/" + uid;
    struct stat st;
    if (stat(user_dir.c_str(), &st) != 0) {
        mkdir(user_dir.c_str(), 0700);
    }

    std::string pass_file = user_dir + "/" + uid + "_pass.txt";
    std::ofstream file(pass_file);
    if (file.is_open()) {
        file << password;
        file.close();
    }
}

void create_user_login_file(const std::string &uid) {
    std::string user_dir = "src/server/storage/USERS/" + uid;
    struct stat st;
    if (stat(user_dir.c_str(), &st) != 0) {
        mkdir(user_dir.c_str(), 0700);
    }

    std::string login_file = user_dir + "/" + uid + "_login.txt";
    std::ofstream file(login_file);
    if (file.is_open()) {
        file << "Logged in\n";
        file.close();
    }
}

void remove_user_login_file(const std::string &uid) {
    std::string login_file = "src/server/storage/USERS/" + uid + "/" + uid + "_login.txt";
    unlink(login_file.c_str());
}

void remove_user_password_file(const std::string &uid) {
    std::string pass_file = "src/server/storage/USERS/" + uid + "/" + uid + "_pass.txt";
    unlink(pass_file.c_str());
}

bool user_password_exists(const std::string &uid) {
    std::string pass_file = "src/server/storage/USERS/" + uid + "/" + uid + "_pass.txt";
    struct stat st;
    return stat(pass_file.c_str(), &st) == 0;
}

bool user_login_exists(const std::string &uid) {
    std::string login_file = "src/server/storage/USERS/" + uid + "/" + uid + "_login.txt";
    struct stat st;
    return stat(login_file.c_str(), &st) == 0;
}

std::string load_user_password(const std::string &uid) {
    std::string pass_file = "src/server/storage/USERS/" + uid + "/" + uid + "_pass.txt";
    std::ifstream file(pass_file);
    if (file.is_open()) {
        std::string password;
        std::getline(file, password);
        file.close();
        return password;
    }
    return "";
}

void create_event_directory(int eid) {
    std::string eid_str = format_eid(eid);
    std::string event_dir = "src/server/storage/EVENTS/" + eid_str;

    struct stat st;
    if (stat(event_dir.c_str(), &st) != 0) {
        mkdir(event_dir.c_str(), 0700);
    }

    std::string reserv_dir = event_dir + "/RESERVATIONS";
    if (stat(reserv_dir.c_str(), &st) != 0) {
        mkdir(reserv_dir.c_str(), 0700);
    }

    std::string desc_dir = event_dir + "/DESCRIPTION";
    if (stat(desc_dir.c_str(), &st) != 0) {
        mkdir(desc_dir.c_str(), 0700);
    }
}

void save_event_start(int eid, const std::string &uid, const std::string &name, const std::string &desc_fname,
                      int event_attend, const std::string &start_date, const std::string &start_time) {
    std::string eid_str = format_eid(eid);
    std::string start_file = "src/server/storage/EVENTS/" + eid_str + "/START_" + eid_str + ".txt";

    std::ofstream file(start_file);
    if (file.is_open()) {
        file << uid << " " << name << " " << desc_fname << " " << event_attend << " " << start_date << " "
             << start_time;
        file.close();
    }
}

void save_event_res_file(int eid, int reserved_seats) {
    std::string eid_str = format_eid(eid);
    std::string res_file = "src/server/storage/EVENTS/" + eid_str + "/RES_" + eid_str + ".txt";

    std::ofstream file(res_file);
    if (file.is_open()) {
        file << reserved_seats;
        file.close();
    }
}

void save_event_description_file(int eid, const std::string &fname, const std::string &fdata) {
    std::string eid_str = format_eid(eid);
    std::string desc_file = "src/server/storage/EVENTS/" + eid_str + "/DESCRIPTION/" + fname;

    std::ofstream file(desc_file, std::ios::binary);
    if (file.is_open()) {
        file << fdata;
        file.close();
    }
}

std::string load_event_description_file(int eid, const std::string &fname) {
    std::string eid_str = format_eid(eid);
    std::string desc_file = "src/server/storage/EVENTS/" + eid_str + "/DESCRIPTION/" + fname;

    std::ifstream file(desc_file, std::ios::binary);
    if (file.is_open()) {
        std::string fdata((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return fdata;
    }
    return "";
}

void save_event_end(int eid, const std::string &end_datetime) {
    std::string eid_str = format_eid(eid);
    std::string end_file = "src/server/storage/EVENTS/" + eid_str + "/END_" + eid_str + ".txt";

    std::ofstream file(end_file);
    if (file.is_open()) {
        file << end_datetime;
        file.close();
    }
}

void add_event_to_user_created(int eid, const std::string &uid) {
    std::string user_dir = "src/server/storage/USERS/" + uid;
    struct stat st;
    if (stat(user_dir.c_str(), &st) != 0) {
        mkdir(user_dir.c_str(), 0700);
    }

    std::string created_dir = user_dir + "/CREATED";
    if (stat(created_dir.c_str(), &st) != 0) {
        mkdir(created_dir.c_str(), 0700);
    }

    std::string eid_str = format_eid(eid);
    std::string created_file = created_dir + "/" + eid_str + ".txt";

    std::ofstream file(created_file);
    if (file.is_open()) {
        file << eid_str;
        file.close();
    }
}

bool event_end_exists(int eid) {
    std::string eid_str = format_eid(eid);
    std::string end_file = "src/server/storage/EVENTS/" + eid_str + "/END_" + eid_str + ".txt";
    struct stat st;
    return stat(end_file.c_str(), &st) == 0;
}

int load_event_reserved_seats(int eid) {
    std::string eid_str = format_eid(eid);
    std::string res_file = "src/server/storage/EVENTS/" + eid_str + "/RES_" + eid_str + ".txt";

    std::ifstream file(res_file);
    if (file.is_open()) {
        int seats;
        file >> seats;
        file.close();
        return seats;
    }
    return 0;
}

std::string load_event_start_info(int eid) {
    std::string eid_str = format_eid(eid);
    std::string start_file = "src/server/storage/EVENTS/" + eid_str + "/START_" + eid_str + ".txt";

    std::ifstream file(start_file);
    if (file.is_open()) {
        std::string line;
        std::getline(file, line);
        file.close();
        return line;
    }
    return "";
}

void save_reservation(int eid, const std::string &uid, int res_num, const std::string &res_datetime) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    char date_buf[11];
    std::strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", &tm);

    char time_buf[7];
    std::strftime(time_buf, sizeof(time_buf), "%H%M%S", &tm);

    std::string eid_str = format_eid(eid);
    std::string reserv_filename = "R-" + uid + "-" + std::string(date_buf) + "_" + std::string(time_buf) + ".txt";

    std::string reserv_file = "src/server/storage/EVENTS/" + eid_str + "/RESERVATIONS/" + reserv_filename;
    std::ofstream file(reserv_file);
    if (file.is_open()) {
        file << uid << " " << res_num << " " << res_datetime;
        file.close();
    }

    add_reservation_to_user_reserved(uid, eid, res_num, res_datetime);
}

void add_reservation_to_user_reserved(const std::string &uid, int /* eid */, int res_num,
                                      const std::string &res_datetime) {
    std::string user_dir = "src/server/storage/USERS/" + uid;
    struct stat st;
    if (stat(user_dir.c_str(), &st) != 0) {
        mkdir(user_dir.c_str(), 0700);
    }

    std::string reserved_dir = user_dir + "/RESERVED";
    if (stat(reserved_dir.c_str(), &st) != 0) {
        mkdir(reserved_dir.c_str(), 0700);
    }

    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    char date_buf[11];
    std::strftime(date_buf, sizeof(date_buf), "%Y-%m-%d", &tm);

    char time_buf[7];
    std::strftime(time_buf, sizeof(time_buf), "%H%M%S", &tm);

    std::string reserv_filename = "R-" + uid + "-" + std::string(date_buf) + "_" + std::string(time_buf) + ".txt";
    std::string reserv_file = reserved_dir + "/" + reserv_filename;

    std::ofstream file(reserv_file);
    if (file.is_open()) {
        file << uid << " " << res_num << " " << res_datetime;
        file.close();
    }
}

bool check_and_close_expired_event(int eid) { return event_end_exists(eid); }

bool event_exists(int eid) {
    std::string eid_str = format_eid(eid);
    std::string start_file = "src/server/storage/EVENTS/" + eid_str + "/START_" + eid_str + ".txt";
    struct stat st;
    return stat(start_file.c_str(), &st) == 0;
}

int find_next_available_eid() {
    int max_eid = 0;
    DIR *events_dir = opendir("src/server/storage/EVENTS");
    if (events_dir != nullptr) {
        struct dirent *entry;
        while ((entry = readdir(events_dir)) != nullptr) {
            if (entry->d_name[0] == '.') continue;

            try {
                int eid = std::stoi(entry->d_name);
                if (eid > max_eid && event_exists(eid)) {
                    max_eid = eid;
                }
            } catch (...) {
            }
        }
        closedir(events_dir);
    }
    return max_eid + 1;
}

std::vector<int> get_all_event_ids() {
    std::vector<int> eids;
    DIR *events_dir = opendir("src/server/storage/EVENTS");
    if (events_dir != nullptr) {
        struct dirent *entry;
        while ((entry = readdir(events_dir)) != nullptr) {
            if (entry->d_name[0] == '.') continue;

            try {
                int eid = std::stoi(entry->d_name);
                if (event_exists(eid)) {
                    eids.push_back(eid);
                }
            } catch (...) {
            }
        }
        closedir(events_dir);
        std::sort(eids.begin(), eids.end());
    }
    return eids;
}

bool parse_event_start_info(const std::string &info, std::string &uid, std::string &name, std::string &desc_fname,
                            int &event_attend, std::string &date_time) {
    std::istringstream ss(info);
    std::string start_date, start_time;

    if (!(ss >> uid >> name >> desc_fname >> event_attend >> start_date >> start_time)) {
        return false;
    }

    date_time = start_date + " " + start_time;
    return true;
}

std::vector<int> get_user_created_events(const std::string &uid) {
    std::vector<int> eids;
    std::string created_dir = "src/server/storage/USERS/" + uid + "/CREATED";

    DIR *dir = opendir(created_dir.c_str());
    if (dir != nullptr) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] == '.') continue;

            std::string filename = entry->d_name;
            size_t dot_pos = filename.find('.');
            if (dot_pos != std::string::npos) {
                std::string eid_str = filename.substr(0, dot_pos);
                try {
                    int eid = std::stoi(eid_str);
                    if (event_exists(eid)) {
                        eids.push_back(eid);
                    }
                } catch (...) {
                }
            }
        }
        closedir(dir);
        std::sort(eids.begin(), eids.end());
    }
    return eids;
}

std::vector<std::tuple<int, std::string, int>> get_user_reservations_from_disk(const std::string &uid) {
    std::vector<std::tuple<int, std::string, int>> reservations;
    std::string reserved_dir = "src/server/storage/USERS/" + uid + "/RESERVED";

    std::vector<int> all_eids = get_all_event_ids();

    DIR *dir = opendir(reserved_dir.c_str());
    if (dir != nullptr) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_name[0] == '.') continue;

            std::string filename = entry->d_name;
            std::string reserv_file = reserved_dir + "/" + filename;
            std::ifstream file(reserv_file);
            if (file.is_open()) {
                std::string uid_from_file, date_part, time_part, res_datetime;
                int res_num;
                if (file >> uid_from_file >> res_num >> date_part >> time_part) {
                    res_datetime = date_part + " " + time_part;

                    for (int eid : all_eids) {
                        std::string eid_str = format_eid(eid);
                        std::string event_reserv_file =
                            "src/server/storage/EVENTS/" + eid_str + "/RESERVATIONS/" + filename;
                        struct stat st;
                        if (stat(event_reserv_file.c_str(), &st) == 0) {
                            reservations.push_back(std::make_tuple(eid, res_datetime, res_num));
                            break;
                        }
                    }
                }
                file.close();
            }
        }
        closedir(dir);
    }
    return reservations;
}
