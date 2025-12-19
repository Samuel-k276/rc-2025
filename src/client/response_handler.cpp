#include "response_handler.h"
#include "session.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

// Handle RLI status - login response (UDP)
// Expected formats: RLI OK\n, RLI REG\n, RLI NOK\n, ERR\n
// Status meanings:
//   OK - user was registered and password matches, user is now logged in
//   REG - user was not registered, ES registered and logged in the user
//   NOK - user exists but password is incorrect
//   ERR - invalid command syntax or parameters
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

// Handle RLO status - logout response (UDP)
// Expected formats: RLO OK\n, RLO NOK\n, RLO UNR\n, RLO WRP\n, ERR\n
// Status meanings:
//   OK - user was logged in and is now logged out
//   NOK - user was not logged in
//   UNR - user was not registered
//   WRP - password is incorrect
//   ERR - invalid command syntax or parameters
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

// Handle RUR status - unregister response (UDP)
// Expected formats: RUR OK\n, RUR NOK\n, RUR UNR\n, RUR WRP\n, ERR\n
// Status meanings:
//   OK - user was registered and logged in, user is now unregistered
//   NOK - user was not logged in
//   UNR - user was not registered
//   WRP - password is incorrect
//   ERR - invalid command syntax or parameters
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

// Handle RCP status - change password response (TCP)
// Expected formats: RCP OK\n, RCP NLG\n, RCP NOK\n, RCP NID\n, ERR\n
// Status meanings:
//   OK - user was logged in and oldPassword matches, password changed to newPassword
//   NLG - user was not logged in
//   NOK - oldPassword is incorrect
//   NID - user does not exist
//   ERR - invalid command syntax or parameters
void handle_change_pass_response(const std::string &response) {
    if (response.substr(0, 7) == "RCP OK\n") {
        std::cout << "successful password change" << std::endl;
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

// Handle RME status [EID state]* - my events response (UDP)
// Expected formats: RME OK EID1 state1 EID2 state2 ...\n, RME NOK\n, RME NLG\n, RME WRP\n, ERR\n
// Status meanings:
//   OK - list of events with their states (state: 1=accepting reservations, 0=past, 2=sold out, 3=closed)
//   NOK - user has not created any events
//   NLG - user is not logged in
//   WRP - password is incorrect
//   ERR - invalid command syntax or parameters
void handle_myevents_response(const std::string &response) {
    if (response.substr(0, 7) == "RME OK ") {
        std::stringstream ss(response);
        std::string status, eid, state;
        ss >> status; // skip "RME"
        ss >> status; // skip "OK"
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

// Handle RMR status [EID date value]* - my reservations response (UDP)
// Expected formats: RMR OK EID1 date1 time1 value1 EID2 date2 time2 value2 ...\n, RMR NOK\n, RMR NLG\n, RMR WRP\n, ERR\n
// Status meanings:
//   OK - list of reservations with EID, date/time (dd-mm-yyyy hh:mm:ss), and number of seats reserved
//   NOK - user has not made any reservations
//   NLG - user is not logged in
//   WRP - password is incorrect
//   ERR - invalid command syntax or parameters
void handle_myreservations_response(const std::string &response) {
    if (response.substr(0, 7) == "RMR OK ") {
        std::stringstream ss(response);
        std::string status, eid, date_part, time_part, value;
        ss >> status >> status; // skip "RMR OK"
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

// Handle RCE status [EID] - create event response (TCP)
// Expected formats: RCE OK EID\n, RCE NOK\n, RCE NLG\n, RCE WRP\n, ERR\n
// Status meanings:
//   OK EID - event created successfully, EID is the unique 3-digit event identifier (001-999)
//   NOK - event could not be created (invalid parameters or no space available)
//   NLG - user is not logged in
//   WRP - password is incorrect
//   ERR - invalid command syntax or parameters
void handle_create_event_response(const std::string &response) {
    if (response.substr(0, 7) == "RCE OK ") {
        std::stringstream ss(response);
        std::string status, eid;
        ss >> status >> status >> eid; // skip "RCE OK", get EID
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

// Handle RCL status - close event response (TCP)
// Expected formats: RCL OK\n, RCL NOK\n, RCL NLG\n, RCL NOE\n, RCL EOW\n, RCL SLD\n, RCL PST\n, RCL CLO\n, ERR\n
// Status meanings:
//   OK - event was open, created by user, and successfully closed
//   NOK - user does not exist or password is incorrect
//   NLG - user is not logged in
//   NOE - event does not exist
//   EOW - event is not owned by user
//   SLD - event is already sold out
//   PST - event date has already passed
//   CLO - event was previously closed by user
//   ERR - invalid command syntax or parameters
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

// Handle RLS status [EID name state event_date]* - list events response (TCP)
// Expected formats: RLS OK EID1 name1 state1 event_date1 EID2 name2 state2 event_date2 ...\n\n, RLS NOK\n, ERR\n
// Status meanings:
//   OK - list of all events with EID, name, state, and event_date (dd-mm-yyyy hh:mm)
//        state: 1=future not sold out, 0=past, 2=future sold out, 3=closed
//        list ends with \n\n
//   NOK - no events have been created yet
//   ERR - invalid command syntax or parameters
void handle_list_events_response(const std::string &response) {
    if (response.substr(0, 7) == "RLS OK ") {
        std::stringstream ss(response);
        std::string status, eid, name, state, date, time;
        ss >> status >> status; // skip "RLS OK"
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

// Handle RSE status [UID name event_date attendance_size Seats_reserved Fname Fsize Fdata] - show event details response (TCP)
// Expected formats: RSE OK UID name event_date attendance_size Seats_reserved Fname Fsize Fdata\n, RSE NOK\n, ERR\n
// Status meanings:
//   OK - event details with owner UID, name, event_date (dd-mm-yyyy hh:mm), total seats, reserved seats, filename, file size, and file data
//   NOK - event does not exist or other error (no file to send)
//   ERR - invalid command syntax or parameters
void handle_show_event_details_response(const std::string &response) {
    if (response.substr(0, 7) == "RSE OK ") {
        std::stringstream ss(response);
        std::string status, ok, uid, name, date_part, time_part, attendance_size, seats_reserved, fname, fsize;
        ss >> status >> ok; // skip "RSE OK"
        ss >> uid >> name >> date_part >> time_part >> attendance_size >> seats_reserved >> fname >> fsize;
        
        std::string event_date = date_part + " " + time_part;

        // File data comes after fname and fsize, need to extract it carefully
        // since fsize might appear elsewhere in the response
        std::string fname_fsize_pattern = fname + " " + fsize + " ";
        size_t pattern_pos = response.find(fname_fsize_pattern);
        std::string fdata;
        if (pattern_pos != std::string::npos) {
            size_t fdata_start = pattern_pos + fname_fsize_pattern.length();
            size_t fdata_end = response.find('\n', fdata_start);
            if (fdata_end == std::string::npos) {
                fdata_end = response.length();
            }
            fdata = response.substr(fdata_start, fdata_end - fdata_start);
        } else {
            // Fallback if pattern matching fails
            size_t fname_pos = response.find(fname);
            if (fname_pos != std::string::npos) {
                size_t search_start = fname_pos + fname.length();
                size_t fsize_pos = response.find(" " + fsize + " ", search_start);
                if (fsize_pos != std::string::npos) {
                    size_t fdata_start = fsize_pos + fsize.length() + 2;
                    size_t fdata_end = response.find('\n', fdata_start);
                    if (fdata_end == std::string::npos) {
                        fdata_end = response.length();
                    }
                    fdata = response.substr(fdata_start, fdata_end - fdata_start);
                }
            }
        }

        int reserved = std::stoi(seats_reserved);
        int total = std::stoi(attendance_size);
        std::string status_info = "";
        if (reserved >= total && total > 0) {
            status_info = " (sold-out)";
        }

        // Save the description file to disk
        std::ofstream file(fname, std::ios::binary);
        if (file.is_open()) {
            file.write(fdata.c_str(), fdata.length());
            file.close();
            char cwd[1024];
            std::string file_path;
            if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                file_path = std::string(cwd) + "/" + fname;
            } else {
                file_path = fname;
            }
            // Print everything on the same line
            std::cout << uid << " " << name << " " << event_date << " " << attendance_size << " " 
                      << seats_reserved << status_info << " " << fname << " " << fsize << " " << file_path << std::endl;
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

// Handle RRI status [n_seats] - reserve response (TCP)
// Expected formats: RRI ACC\n, RRI REJ n_seats\n, RRI NOK\n, RRI NLG\n, RRI CLO\n, RRI SLD\n, RRI PST\n, RRI WRP\n, ERR\n
// Status meanings:
//   ACC - reservation accepted, can be fulfilled
//   REJ n_seats - reservation rejected, requested places > remaining places, n_seats is the number of remaining seats
//   NOK - event is not active
//   NLG - user is not logged in
//   CLO - event is closed
//   SLD - event is sold out
//   PST - event date has passed
//   WRP - password is incorrect
//   ERR - invalid command syntax or parameters
void handle_reserve_response(const std::string &response) {
    if (response.substr(0, 8) == "RRI ACC\n") {
        std::cout << "accepted" << std::endl;
    } else if (response.substr(0, 7) == "RRI REJ ") {
        std::stringstream ss(response);
        std::string status, rej, n_seats;
        ss >> status >> rej >> n_seats; // get remaining seats count
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
