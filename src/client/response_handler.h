#ifndef RESPONSE_HANDLER_H
#define RESPONSE_HANDLER_H

#include <string>

/**
 * Process login response from server
 * @param response: server response
 */
void handle_login_response(const std::string &response);

/**
 * Process logout response from server
 * @param response: server response
 */
void handle_logout_response(const std::string &response);

/**
 * Process unregister response from server
 * @param response: server response
 */
void handle_unregister_response(const std::string &response);

/**
 * Process change password response from server
 * @param response: server response
 */
void handle_change_pass_response(const std::string &response);

/**
 * Process myevents response from server
 * @param response: server response
 */
void handle_myevents_response(const std::string &response);

/**
 * Process myreservations response from server
 * @param response: server response
 */
void handle_myreservations_response(const std::string &response);

/**
 * Process create event response from server
 * @param response: server response
 */
void handle_create_event_response(const std::string &response);

/**
 * Process close event response from server
 * @param response: server response
 */
void handle_close_event_response(const std::string &response);

/**
 * Process list events response from server
 * @param response: server response
 */
void handle_list_events_response(const std::string &response);

/**
 * Process show event details response from server
 * @param response: server response
 */
void handle_show_event_details_response(const std::string &response);

/**
 * Process reserve response from server
 * @param response: server response
 */
void handle_reserve_response(const std::string &response);

#endif // RESPONSE_HANDLER_H
