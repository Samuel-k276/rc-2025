#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdbool>
#include <pthread.h>

#include "tcp_server.h"
#include "udp_server.h"

/** 
 * Server port
 * Group 11 + 50000 is the default port for the server.
 */
char *port = (char *)"58011";

/** 
 * Verbose mode
 * It outputs to the screen a short description of the received requests
 * (UID, type of request), and the IP and port originating those requests.
 */
bool verbose = false;

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "p:v")) != -1) {
        switch (opt) {
            case 'p':
                port = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-p <ESport>] [-v]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    std::cout << "Server is running on port " << port << std::endl;
    std::cout << "Verbose mode: " << (verbose ? "on" : "off") << std::endl;

    // Create threads for TCP and UDP servers
    pthread_t tcp_thread, udp_thread;

    if (pthread_create(&tcp_thread, nullptr, tcp_server_thread, port) != 0) {
        std::cerr << "Failed to create TCP thread" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&udp_thread, nullptr, udp_server_thread, port) != 0) {
        std::cerr << "Failed to create UDP thread" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Wait for threads to complete (they run indefinitely)
    pthread_join(tcp_thread, nullptr);
    pthread_join(udp_thread, nullptr);

    return 0;
}
