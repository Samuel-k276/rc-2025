#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdbool>

int main(int argc, char *argv[]) {
    char *ESIP = (char *)"127.0.0.1";
    char *ESport = (char *)"58011"; // Group 11 + 50000
    int opt;

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

    return 0;
}