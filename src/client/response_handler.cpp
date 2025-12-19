#include "response_handler.h"
#include "session.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>

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
    if (response.substr(0, 7) == "UNR OK\n") {
        std::cout << "successful unregister" << std::endl;
        clear_user_session();
    } else if (response.substr(0, 8) == "UNR UNR\n") {
        std::cout << "unknown user" << std::endl;
    } else if (response.substr(0, 8) == "UNR NOK\n" || response.substr(0, 8) == "UNR WRP\n") {
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
            if (state == "1") state_desc = "accepting reservations";
            else if (state == "0") state_desc = "past";
            else if (state == "2") state_desc = "sold out";
            else if (state == "3") state_desc = "closed";
            else state_desc = "unknown";
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
        // Parse and display reservations: RMR OK EID1 timestamp1 value1 EID2 timestamp2 value2 ...
        // timestamp format from ctime(): "Day Mon DD HH:MM:SS YYYY" (e.g., "Mon Nov 18 14:30:00 2024")
        std::stringstream ss(response);
        std::string status, eid, value;
        ss >> status >> status; // RMR OK
        while (ss >> eid) {
            // Read timestamp (ctime format has spaces, read until we find a number for value)
            std::string token;
            std::string full_timestamp;
            // Read tokens until we find a pure number (the value/seats)
            while (ss >> token) {
                // Check if token is a pure number (the value)
                bool is_number = true;
                for (char c : token) {
                    if (!std::isdigit(c)) {
                        is_number = false;
                        break;
                    }
                }
                if (is_number && !full_timestamp.empty()) {
                    value = token;
                    break;
                }
                if (!full_timestamp.empty()) full_timestamp += " ";
                full_timestamp += token;
            }
            std::cout << "Reservation: Event " << eid << " - " << full_timestamp << " (" << value << " seats)" << std::endl;
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
            if (state == "1") state_desc = "future, not sold out";
            else if (state == "0") state_desc = "past";
            else if (state == "2") state_desc = "future, sold out";
            else if (state == "3") state_desc = "closed";
            else state_desc = "unknown";
            std::cout << "Event " << eid << ": " << name << " (" << state_desc << ") - " << date << " " << time << std::endl;
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
        std::stringstream ss(response);
        std::string status, uid, name, event_date, attendance_size, seats_reserved, fname, fsize;
        ss >> status >> status; // RSE OK
        ss >> uid >> name >> event_date >> attendance_size >> seats_reserved >> fname >> fsize;
        
        // Extract file data (rest of the response)
        std::string fdata;
        std::getline(ss, fdata);
        // Remove leading space if present
        if (!fdata.empty() && fdata[0] == ' ') {
            fdata = fdata.substr(1);
        }
        
        // Display event details
        std::cout << "Event Details:" << std::endl;
        std::cout << "  Owner UID: " << uid << std::endl;
        std::cout << "  Name: " << name << std::endl;
        std::cout << "  Date: " << event_date << std::endl;
        std::cout << "  Total Seats: " << attendance_size << std::endl;
        std::cout << "  Reserved Seats: " << seats_reserved << std::endl;
        std::cout << "  File: " << fname << " (" << fsize << " bytes)" << std::endl;
        
        // Save file locally
        std::ofstream file(fname, std::ios::binary);
        if (file.is_open()) {
            file.write(fdata.c_str(), fdata.length());
            file.close();
            std::cout << "File saved: " << fname << std::endl;
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
    if (response.substr(0, 7) == "RRI ACC\n") {
        std::cout << "accepted" << std::endl;
    } else if (response.substr(0, 7) == "RRI REJ ") {
        // Extract remaining seats: RRI REJ n_seats\n
        std::stringstream ss(response);
        std::string status, rej, n_seats;
        ss >> status >> rej >> n_seats;
        std::cout << "refused (remaining seats: " << n_seats << ")" << std::endl;
    } else if (response.substr(0, 8) == "RRI NOK\n") {
        std::cout << "event is no longer active" << std::endl;
    } else if (response.substr(0, 8) == "RRI NLG\n") {
        std::cerr << "User is not logged in" << std::endl;
    } else if (response.substr(0, 8) == "RRI CLS\n") {
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

