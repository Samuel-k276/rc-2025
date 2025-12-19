#include "utils.h"
#include <arpa/inet.h>
#include <iostream>

void print_verbose_message(std::string uid, std::string command_code, struct sockaddr_in &client_addr, bool verbose) {
    if (verbose) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), ip, INET_ADDRSTRLEN);
        std::cout << uid << " " << command_code << " " << ip << " " << ntohs(client_addr.sin_port) << std::endl;
    }
}

