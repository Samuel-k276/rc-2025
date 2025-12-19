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
#include "command.h"
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

    int TCP_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    int UDP_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (UDP_socket_fd == -1 || TCP_socket_fd == -1 ) {
        std::cerr << "Failed to create socket" << std::endl;
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
                    std::cerr << "Invalid login arguments" << std::endl;
                    break;
                }
                if (!send_udp_command(UDP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send login command to server" << std::endl;
                    break;
                }
                if (response == "RLI OK\n") {
                    std::cout << "successful login" << std::endl;
                    promote_temp_user_to_user();    
                } else if (response == "RLI REG\n") {
                    std::cout << "new user registered" << std::endl;
                    promote_temp_user_to_user();
                } else if (response == "RLI NOK\n") {
                    std::cout << "incorrect login attempt" << std::endl;
                    clear_temp_user_session();
                } else {
                    std::cerr << "Unexpected login response: " << response << std::endl;
                    clear_temp_user_session();
                }
                break;
            }
            case LOGOUT: {
                if (!parse_logout_input(args, message)) {
                    std::cerr << "Invalid logout arguments" << std::endl;
                    break;
                }
                std::string response;
                if (!send_udp_command(UDP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send logout command to server" << std::endl;
                    break;
                }
                if (response == "RLO OK\n") {
                    std::cout << "successful logout" << std::endl;
                    clear_user_session();
                } else if (response == "RLO UNR\n") {
                    std::cout << "unknown user" << std::endl;
                } else if (response == "RLO NOK\n") {
                    std::cout << "user not logged in" << std::endl;
                } else {
                    std::cerr << "Unexpected logout response: " << response << std::endl;
                }
                break;
            }
            case UNREGISTER: {
                if (!parse_unregister_input(args, message)) {
                    std::cerr << "Invalid unregister arguments" << std::endl;
                    break;
                }
                if (!send_udp_command(UDP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send unregister command to server" << std::endl;
                    break;
                }
                if (response == "UNR OK\n") {
                    std::cout << "successful unregister" << std::endl;
                    clear_user_session();
                } else if (response == "UNR UNR\n") {
                    std::cout << "unknown user" << std::endl;
                } else if (response == "UNR NOK\n" || response == "UNR WRP\n") {
                    std::cout << "incorrect unregister attempt" << std::endl;
                } else {
                    std::cerr << "Unexpected unregister response: " << response << std::endl;
                }
                break;
            }
            case MYEVENTS: {
                if(!parse_myevents_input(args, message)) {
                    std::cerr << "Invalid myevents arguments" << std::endl;
                    break;
                }
                if (!send_udp_command(UDP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send myevents command to server" << std::endl;
                    break;
                }
                if (response == "LME OK\n") {
                    std::cout << "My events successful" << std::endl;
                    break;
                } else if (response == "LME NOK\n") {
                    std::cerr << "Failed to get my events" << std::endl;
                    break;
                } else if (response == "LME ERR\n") {
                    std::cerr << "Error getting my events" << std::endl;
                    break;
                }
                break;
            }
            case EXIT:
                if (!parse_exit_input(args)) {
                    std::cerr << "Invalid exit arguments" << std::endl;
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
                    std::cerr << "Invalid create event arguments" << std::endl;
                    break;
                }
                if (!send_tcp_command(TCP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send create event command to server" << std::endl;
                    break;
                }
                if (response == "CRE OK\n") {
                    std::cout << "Event created successfully" << std::endl;
                    break;
                } else if (response == "CRE NOK\n") {
                    std::cerr << "Failed to create event" << std::endl;
                    break;
                } else if (response == "CRE ERR\n") {
                    std::cerr << "Error creating event" << std::endl;
                    break;
                }
                break;
            }
            case CLOSE_EVENT:
                if (!parse_close_event_input(args, message)) {
                    std::cerr << "Invalid close event arguments" << std::endl;
                    break;
                }
                if (!send_tcp_command(TCP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send close event command to server" << std::endl;
                    break;
                }
                if (response == "CLS OK\n") {
                    std::cout << "Event closed successfully" << std::endl;
                    break;
                } else if (response == "CLS NOK\n") {
                    std::cerr << "Failed to close event" << std::endl;
                    break;
                } else if (response == "CLS ERR\n") {
                    std::cerr << "Error closing event" << std::endl;
                    break;
                }
                break;
            case LIST_EVENTS:
                if (!parse_list_events_input(args, message)) {
                    std::cerr << "Invalid list events arguments" << std::endl;
                    break;
                }
                if (!send_tcp_command(TCP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send list events command to server" << std::endl;
                    break;
                }
                break;
            case SHOW_EVENT_DETAILS:
                if (!parse_show_event_details_input(args, message)) {
                    std::cerr << "Invalid show event details arguments" << std::endl;
                    break;
                }
                if (!send_tcp_command(TCP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send show event details command to server" << std::endl;
                    break;
                }
                break;
            case RESERVE:
                if (!parse_reserve_input(args, message)) {
                    std::cerr << "Invalid reserve arguments" << std::endl;
                    break;
                }
                if (!send_tcp_command(TCP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send reserve command to server" << std::endl;
                    break;
                }
                break;
            case CHANGE_PASS:
                if (!parse_change_pass_input(args, message)) {
                    std::cerr << "Invalid change pass arguments" << std::endl;
                    break;
                }
                if (!send_tcp_command(TCP_socket_fd, message, res, response)) {
                    std::cerr << "Failed to send change pass command to server" << std::endl;
                    break;
                }
                if (response == "CPS OK\n") {
                    std::cout << "Password changed successfully" << std::endl;
                    break;
                } else if (response == "CPS NOK\n") {
                    std::cerr << "Failed to change password" << std::endl;
                    break;
                } else if (response == "CPS ERR\n") {
                    std::cerr << "Error changing password" << std::endl;
                    break;
                }
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