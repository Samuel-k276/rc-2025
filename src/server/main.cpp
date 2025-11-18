#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdbool>


int main(int argc, char *argv[]) {
    char *ESport = (char *)"58011"; // Group 11 + 50000
    bool is_verbose = false;    
    int opt;

    while ((opt = getopt(argc, argv, "p:v")) != -1) {
        switch (opt) {
            case 'p':
                ESport = optarg;
                break;
            case 'v':
                is_verbose = true;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-p <ESport>] [-v]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    std::cout << "Server is running on port " << ESport << std::endl;
    std::cout << "Verbose mode: " << (is_verbose ? "on" : "off") << std::endl;
    return 0;
}
