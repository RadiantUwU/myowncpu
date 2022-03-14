#include "SerialPort.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <string>
#include <chrono>
#include <thread>
#define or ||
#define and &&
SerialPort *arduino;
void signalHandler(int signum) {
    std::cout << "Terminated." << std::endl;
    if (arduino->isConnected()) arduino->closeSerial();
    exit(signum);
}
char *portName = nullptr;
char *ROMFileName = nullptr;
int main(int argc, char** argv) {
    bool makingport = false;
    bool makingrom = false;
    signal(SIGINT,signalHandler);
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'p':
                    makingport = true;
                    break;
                case 'r':
                    makingrom = true;
                    break;
            }
            continue;
        }
        if (makingport) {
            portName = argv[i];
            makingport = false;
        } else if (makingrom) {
            ROMFileName = argv[i];
            makingrom = false;
        }
    }
    if (portName == nullptr or ROMFileName == nullptr) {
        std::cerr<< "PortName and/or RomFileName are missing"<< std::endl;
        std::cout<< "Terminated."<< std::endl;
        return -1;
    }
    arduino = new SerialPort(portName);
    if (arduino->isConnected()) {
        std::cout<< "Connection established."<< std::endl;
    } else {
        std::cerr<< "Failed to connect to port."<< std::endl;
        std::cout<< "Terminated."<< std::endl;
        return -1;
    }
    std::cout<< "Terminated."<< std::endl;
    arduino->closeSerial();
    return 0;
}