#include <arpa/inet.h>
#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../common/constants.h"
#include "input_handler.h"
#include "response_handler.h"
#include "send.h"
#include "session.h"

struct addrinfo hints, *res;

char *ESIP = (char *)"127.0.0.1";
char *ESport = (char *)"58011"; // Group 11 + 50000

/**
 * Get the command and arguments from a buffer with return.
 * @param buffer: buffer
 * @param command: command
 * @return std::stringstream: the stream with the arguments
 */
std::stringstream get_command_and_args_with_return(char *buffer, std::string &command) {
    std::stringstream ss(buffer);
    if (ss >> command) {
        return ss;
    }
    return std::stringstream("");
}

int main(int argc, char *argv[]) {
    int opt;
    struct addrinfo hints, *res;
    char buffer[MAX_MESSAGE_LENGTH];
    while ((opt = getopt(argc, argv, "n:p:")) != -1) {
        switch (opt) {
            case 'n':
                ESIP = optarg;
                break;
            case 'p':
                ESport = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-n <ESIP>] [-p <ESport>]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    std::cout << "Connecting to server at " << ESIP << ":" << ESport << std::endl;

    int UDP_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (UDP_socket_fd == -1) {
        std::cerr << "Failed to create UDP socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    // hints.ai_socktype = SOCK_STREAM; // TCP socket

    int error = getaddrinfo(ESIP, ESport, &hints, &res);
    if (error != 0) {
        std::cerr << "Failed to get address info: " << gai_strerror(error) << std::endl;
        exit(EXIT_FAILURE);
    }

    while (true) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            std::cout << "Input ended. Exiting." << std::endl;
            break;
        }
        std::string command;
        std::stringstream args = get_command_and_args_with_return(buffer, command);
        if (!is_valid_client_command(command)) {
            std::cerr << "Unknown command: " << command << std::endl;
            continue;
        }

        UserCommand user_command = get_command(command);
        std::string message, response;

        switch (user_command) {
            case LOGIN: {
                if (is_user_logged_in()) {
                    std::cerr << "User is already logged in, cannot login again" << std::endl;
                    break;
                }
                if (!parse_login_input(args, message)) {
                    break;
                }
                if (!send_udp_command(UDP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send login command to server" << std::endl;
                    break;
                }
                handle_login_response(response);
                break;
            }
            case LOGOUT: {
                if (!parse_logout_input(args, message)) {
                    break;
                }
                std::string response;
                if (!send_udp_command(UDP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send logout command to server" << std::endl;
                    break;
                }
                handle_logout_response(response);
                break;
            }
            case UNREGISTER: {
                if (!parse_unregister_input(args, message)) {
                    break;
                }
                if (!send_udp_command(UDP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send unregister command to server" << std::endl;
                    break;
                }
                handle_unregister_response(response);
                break;
            }
            case MYEVENTS: {
                if (!parse_myevents_input(args, message)) {
                    break;
                }
                if (!send_udp_command(UDP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send myevents command to server" << std::endl;
                    break;
                }
                handle_myevents_response(response);
                break;
            }
            case MYRESERVATIONS: {
                std::string extra;
                if (args >> extra) {
                    std::cerr << "Invalid myreservations arguments" << std::endl;
                    break;
                }
                std::string uid = get_user_id();
                std::string password = get_user_password();
                message = command_to_string.at(MYRESERVATIONS) + " " + uid + " " + password + "\n";
                if (!send_udp_command(UDP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send myreservations command to server" << std::endl;
                    break;
                }
                handle_myreservations_response(response);
                break;
            }
            case EXIT:
                if (!parse_exit_input(args)) {
                    break;
                }
                if (is_user_logged_in()) {
                    std::cerr << "User is logged in, cannot exit, please logout first" << std::endl;
                    break;
                }
                std::cout << "Exiting program" << std::endl;
                exit(EXIT_SUCCESS);
                break;
            case CREATE_EVENT: {
                if (!parse_create_event_input(args, message)) {
                    break;
                }
                if (!send_tcp_command(message, res, response)) {
                    std::cerr << "Failed to send create event command to server" << std::endl;
                    break;
                }
                handle_create_event_response(response);
                break;
            }
            case CLOSE_EVENT:
                if (!parse_close_event_input(args, message)) {
                    break;
                }
                if (!send_tcp_command(message, res, response)) {
                    std::cerr << "Failed to send close event command to server" << std::endl;
                    break;
                }
                handle_close_event_response(response);
                break;
            case LIST_EVENTS:
                if (!parse_list_events_input(args, message)) {
                    break;
                }
                if (!send_tcp_command(message, res, response)) {
                    std::cerr << "Failed to send list events command to server" << std::endl;
                    break;
                }
                handle_list_events_response(response);
                break;
            case SHOW_EVENT_DETAILS:
                if (!parse_show_event_details_input(args, message)) {
                    break;
                }
                if (!send_tcp_command(message, res, response)) {
                    std::cerr << "Failed to send show event details command to server" << std::endl;
                    break;
                }
                handle_show_event_details_response(response);
                break;
            case RESERVE:
                if (!parse_reserve_input(args, message)) {
                    break;
                }
                if (!send_tcp_command(message, res, response)) {
                    std::cerr << "Failed to send reserve command to server" << std::endl;
                    break;
                }
                handle_reserve_response(response);
                break;
            case CHANGE_PASS:
                if (!parse_change_pass_input(args, message)) {
                    break;
                }
                if (!send_tcp_command(message, res, response)) {
                    std::cerr << "Failed to send change pass command to server" << std::endl;
                    break;
                }
                handle_change_pass_response(response);
                break;
            default:
                std::cerr << "Unknown command: " << command << std::endl;
                break;
        }
    }

    freeaddrinfo(res);
    close(UDP_socket_fd);
    return 0;
}