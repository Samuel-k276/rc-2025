#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <string>

// Handle responses from the server and update client state accordingly
void handle_login_response(const std::string &response);
void handle_logout_response(const std::string &response);
void handle_unregister_response(const std::string &response);
void handle_change_pass_response(const std::string &response);
void handle_myevents_response(const std::string &response);
void handle_myreservations_response(const std::string &response);
void handle_create_event_response(const std::string &response);
void handle_close_event_response(const std::string &response);
void handle_list_events_response(const std::string &response);
void handle_show_event_details_response(const std::string &response);
void handle_reserve_response(const std::string &response);

#endif // RESPONSE_HANDLER_H
