#ifndef SEND_H
#define SEND_H

#include "command.h"
#include <netdb.h>
#include <string>

/**
 * Open a TCP connection to the server and send a command.
 * @param socket_fd: socket file descriptor
 * @param buffer: command message
 * @param res: address info result
 * @param response: response from server
 * @return bool: true if the command is sent successfully, false otherwise
 */
bool send_tcp_command(int socket_fd, std::string &buffer, struct addrinfo *res, std::string &response);

/**
 * Uses the open UDP socket to send a command.
 * @param socket_fd: socket file descriptor
 * @param message: command message
 * @param res: address info result
 * @return bool: true if the command is sent successfully, false otherwise
 */
bool send_udp_command(int socket_fd, std::string &message, struct addrinfo *res, std::string &response);

#endif