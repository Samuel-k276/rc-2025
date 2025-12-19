#ifndef UTILS_H
#define UTILS_H

#include <netinet/in.h>
#include <string>

void print_verbose_message(std::string uid, std::string command_code, struct sockaddr_in &client_addr, bool verbose);

#endif

