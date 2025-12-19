#include "response_handler.h"
#include "session.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

void handle_login_response(const std::string &response) {
    if (response.substr(0, 7) == "RLI OK\n") {
        std::cout << "successful login" << std::endl;
        promote_temp_user_to_user();
    } else if (response.substr(0, 8) == "RLI REG\n") {
        std::cout << "new user registered" << std::endl;
        promote_temp_user_to_user();
    } else if (response.substr(0, 8) == "RLI NOK\n") {
        std::cout << "incorrect login attempt" << std::endl;
        clear_temp_user_session();
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
        clear_temp_user_session();
    } else {
        std::cerr << "Unexpected login response: " << response << std::endl;
        clear_temp_user_session();
    }
}

void handle_logout_response(const std::string &response) {
    if (response.substr(0, 7) == "RLO OK\n") {
        std::cout << "successful logout" << std::endl;
        clear_user_session();
    } else if (response.substr(0, 8) == "RLO UNR\n") {
        std::cout << "unknown user" << std::endl;
    } else if (response.substr(0, 8) == "RLO NOK\n") {
        std::cout << "user not logged in" << std::endl;
    } else if (response.substr(0, 8) == "RLO WRP\n") {
        std::cerr << "Password is incorrect" << std::endl;
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
    } else {
        std::cerr << "Unexpected logout response: " << response << std::endl;
    }
}

void handle_unregister_response(const std::string &response) {
    if (response.substr(0, 7) == "RUR OK\n") {
        std::cout << "successful unregister" << std::endl;
        clear_user_session();
    } else if (response.substr(0, 8) == "RUR UNR\n") {
        std::cout << "unknown user" << std::endl;
    } else if (response.substr(0, 8) == "RUR NOK\n" || response.substr(0, 8) == "RUR WRP\n") {
        std::cout << "incorrect unregister attempt" << std::endl;
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
    } else {
        std::cerr << "Unexpected unregister response: " << response << std::endl;
    }
}

void handle_change_pass_response(const std::string &response) {
    if (response.substr(0, 7) == "RCP OK\n") {
        std::cout << "successful password change" << std::endl;
        // Update local session password (same logic as login - promote temp to user)
        promote_temp_user_to_user();
    } else if (response.substr(0, 8) == "RCP NLG\n") {
        std::cout << "user not logged In" << std::endl;
        clear_temp_user_session();
    } else if (response.substr(0, 8) == "RCP NOK\n") {
        std::cout << "incorrect password" << std::endl;
        clear_temp_user_session();
    } else if (response.substr(0, 8) == "RCP NID\n") {
        std::cout << "unknown user" << std::endl;
        clear_temp_user_session();
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
        clear_temp_user_session();
    } else {
        std::cerr << "Unexpected change pass response: " << response << std::endl;
        clear_temp_user_session();
    }
}

void handle_myevents_response(const std::string &response) {
    if (response.substr(0, 7) == "RME OK ") {
        // Parse and display events: RME OK EID1 state1 EID2 state2 ...
        std::stringstream ss(response);
        std::string status, eid, state;
        ss >> status; // RME
        ss >> status; // OK
        while (ss >> eid >> state) {
            std::string state_desc;
            if (state == "1")
                state_desc = "accepting reservations";
            else if (state == "0")
                state_desc = "past";
            else if (state == "2")
                state_desc = "sold out";
            else if (state == "3")
                state_desc = "closed";
            else
                state_desc = "unknown";
            std::cout << "Event " << eid << ": " << state_desc << std::endl;
        }
    } else if (response.substr(0, 8) == "RME NOK\n") {
        std::cout << "user has not created any events" << std::endl;
    } else if (response.substr(0, 8) == "RME NLG\n") {
        std::cerr << "User is not logged in" << std::endl;
    } else if (response.substr(0, 8) == "RME WRP\n") {
        std::cerr << "Password is incorrect" << std::endl;
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
    } else {
        std::cerr << "Unexpected myevents response: " << response << std::endl;
    }
}

void handle_myreservations_response(const std::string &response) {
    if (response.substr(0, 7) == "RMR OK ") {
        // Parse and display reservations: RMR OK EID1 date1 time1 value1 EID2 date2 time2 value2 ...
        // timestamp format: "dd-mm-yyyy hh:mm:ss" (e.g., "19-12-2025 20:04:53")
        std::stringstream ss(response);
        std::string status, eid, date_part, time_part, value;
        ss >> status >> status; // RMR OK
        while (ss >> eid >> date_part >> time_part >> value) {
            std::string timestamp = date_part + " " + time_part;
            std::cout << "Reservation: Event " << eid << " - " << timestamp << " (" << value << " seats)"
                      << std::endl;
        }
    } else if (response.substr(0, 8) == "RMR NOK\n") {
        std::cout << "user has not made any reservation" << std::endl;
    } else if (response.substr(0, 8) == "RMR NLG\n") {
        std::cerr << "User is not logged in" << std::endl;
    } else if (response.substr(0, 8) == "RMR WRP\n") {
        std::cerr << "Password is incorrect" << std::endl;
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
    } else {
        std::cerr << "Unexpected myreservations response: " << response << std::endl;
    }
}

void handle_create_event_response(const std::string &response) {
    if (response.substr(0, 7) == "RCE OK ") {
        // Extract EID from response: RCE OK EID\n
        std::stringstream ss(response);
        std::string status, eid;
        ss >> status >> status >> eid; // RCE OK EID
        std::cout << "Event created successfully with EID: " << eid << std::endl;
    } else if (response.substr(0, 8) == "RCE NOK\n") {
        std::cerr << "Failed to create event (invalid parameters or no space)" << std::endl;
    } else if (response.substr(0, 8) == "RCE NLG\n") {
        std::cerr << "User is not logged in" << std::endl;
    } else if (response.substr(0, 8) == "RCE WRP\n") {
        std::cerr << "Password is incorrect" << std::endl;
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
    } else {
        std::cerr << "Unexpected create event response: " << response << std::endl;
    }
}

void handle_close_event_response(const std::string &response) {
    if (response.substr(0, 7) == "RCL OK\n") {
        std::cout << "Event closed successfully" << std::endl;
    } else if (response.substr(0, 8) == "RCL NOK\n") {
        std::cerr << "User does not exist or password is incorrect" << std::endl;
    } else if (response.substr(0, 8) == "RCL NLG\n") {
        std::cerr << "User is not logged in" << std::endl;
    } else if (response.substr(0, 8) == "RCL NOE\n") {
        std::cerr << "Event does not exist" << std::endl;
    } else if (response.substr(0, 8) == "RCL EOW\n") {
        std::cerr << "Event is not owned by user" << std::endl;
    } else if (response.substr(0, 8) == "RCL SLD\n") {
        std::cout << "event is sold-out" << std::endl;
    } else if (response.substr(0, 8) == "RCL PST\n") {
        std::cout << "event has already expired" << std::endl;
    } else if (response.substr(0, 8) == "RCL CLO\n") {
        std::cerr << "Event was previously closed" << std::endl;
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
    } else {
        std::cerr << "Unexpected close event response: " << response << std::endl;
    }
}

void handle_list_events_response(const std::string &response) {
    if (response.substr(0, 7) == "RLS OK ") {
        // Parse and display events: RLS OK EID1 name1 state1 event_date1 EID2 name2 state2 event_date2 ...\n\n
        std::stringstream ss(response);
        std::string status, eid, name, state, date, time;
        ss >> status >> status; // RLS OK
        while (ss >> eid >> name >> state >> date >> time) {
            std::string state_desc;
            if (state == "1")
                state_desc = "future, not sold out";
            else if (state == "0")
                state_desc = "past";
            else if (state == "2")
                state_desc = "future, sold out";
            else if (state == "3")
                state_desc = "closed";
            else
                state_desc = "unknown";
            std::cout << "Event " << eid << ": " << name << " (" << state_desc << ") - " << date << " " << time
                      << std::endl;
        }
    } else if (response.substr(0, 8) == "RLS NOK\n") {
        std::cout << "no events has yet been created" << std::endl;
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax" << std::endl;
    } else {
        std::cerr << "Unexpected list events response: " << response << std::endl;
    }
}

void handle_show_event_details_response(const std::string &response) {
    if (response.substr(0, 7) == "RSE OK ") {
        // Parse response: RSE OK UID name event_date attendance_size Seats_reserved Fname Fsize Fdata
        // Note: event_date contains space (format: "dd-mm-yyyy hh:mm")
        std::stringstream ss(response);
        std::string status, ok, uid, name, date_part, time_part, attendance_size, seats_reserved, fname, fsize;
        ss >> status >> ok; // RSE OK
        ss >> uid >> name >> date_part >> time_part >> attendance_size >> seats_reserved >> fname >> fsize;
        
        std::string event_date = date_part + " " + time_part;

        // Extract file data (rest of the response after fsize)
        // Find position after "fname fsize " pattern to avoid finding fsize elsewhere
        std::string fname_fsize_pattern = fname + " " + fsize + " ";
        size_t pattern_pos = response.find(fname_fsize_pattern);
        std::string fdata;
        if (pattern_pos != std::string::npos) {
            size_t fdata_start = pattern_pos + fname_fsize_pattern.length();
            // Read until newline (if present)
            size_t fdata_end = response.find('\n', fdata_start);
            if (fdata_end == std::string::npos) {
                fdata_end = response.length();
            }
            fdata = response.substr(fdata_start, fdata_end - fdata_start);
        } else {
            // Fallback: try to find fsize after fname
            size_t fname_pos = response.find(fname);
            if (fname_pos != std::string::npos) {
                size_t search_start = fname_pos + fname.length();
                size_t fsize_pos = response.find(" " + fsize + " ", search_start);
                if (fsize_pos != std::string::npos) {
                    size_t fdata_start = fsize_pos + fsize.length() + 2; // +2 for " " and " "
                    size_t fdata_end = response.find('\n', fdata_start);
                    if (fdata_end == std::string::npos) {
                        fdata_end = response.length();
                    }
                    fdata = response.substr(fdata_start, fdata_end - fdata_start);
                }
            }
        }

        // Determine event status: sold-out if reserved >= total
        int reserved = std::stoi(seats_reserved);
        int total = std::stoi(attendance_size);
        std::string status_info = "";
        if (reserved >= total && total > 0) {
            status_info = " (sold-out)";
        }
        // Note: We can't determine if event is closed from the response alone
        // The server would need to include this info, but for now we only show sold-out

        // Display event details: UID name event_date attendance_size Seats_reserved [status]
        std::cout << uid << " " << name << " " << event_date << " " << attendance_size << " " 
                  << seats_reserved << status_info << std::endl;

        // Save file locally and display file info
        std::ofstream file(fname, std::ios::binary);
        if (file.is_open()) {
            file.write(fdata.c_str(), fdata.length());
            file.close();
            // Get current directory for display
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                std::cout << fname << " " << fsize << " " << cwd << "/" << fname << std::endl;
            } else {
                std::cout << fname << " " << fsize << " " << fname << std::endl;
            }
        } else {
            std::cerr << "Failed to save file: " << fname << std::endl;
        }
    } else if (response.substr(0, 8) == "RSE NOK\n") {
        std::cerr << "Event does not exist or other error" << std::endl;
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
    } else {
        std::cerr << "Unexpected show event details response: " << response << std::endl;
    }
}

void handle_reserve_response(const std::string &response) {
    if (response.substr(0, 8) == "RRI ACC\n") {
        std::cout << "accepted" << std::endl;
    } else if (response.substr(0, 7) == "RRI REJ ") {
        // Parse RRI REJ n_seats\n
        std::stringstream ss(response);
        std::string status, rej, n_seats;
        ss >> status >> rej >> n_seats;
        std::cout << "refused " << n_seats << std::endl;
    } else if (response.substr(0, 8) == "RRI NOK\n") {
        std::cout << "event is no longer active" << std::endl;
    } else if (response.substr(0, 8) == "RRI NLG\n") {
        std::cerr << "User is not logged in" << std::endl;
    } else if (response.substr(0, 8) == "RRI CLO\n") {
        std::cout << "event is no longer active" << std::endl;
    } else if (response.substr(0, 8) == "RRI SLD\n") {
        std::cout << "event is no longer active" << std::endl;
    } else if (response.substr(0, 8) == "RRI PST\n") {
        std::cout << "event is no longer active" << std::endl;
    } else if (response.substr(0, 8) == "RRI WRP\n") {
        std::cerr << "Password is incorrect" << std::endl;
    } else if (response.substr(0, 4) == "ERR\n") {
        std::cerr << "Invalid command syntax or parameters" << std::endl;
    } else {
        std::cerr << "Unexpected reserve response: " << response << std::endl;
    }
}
