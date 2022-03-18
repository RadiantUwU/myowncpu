#include "SerialPort.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#define or ||
#define and &&
SerialPort *arduino;
std::ifstream file;
void signalHandler(int signum) {
    std::cout << "Terminated." << std::endl;
    if (arduino->isConnected()) arduino->closeSerial();
    if (file.is_open()) file.close();
    exit(signum);
}
struct SerialInst {
    char inst[3];
    SerialInst(char* inst) {
        this->inst[0] = inst[0];
        this->inst[1] = inst[1];
        this->inst[2] = inst[2];
    }
    SerialInst(char inst[3]) {
        this->inst[0] = inst[0];
        this->inst[1] = inst[1];
        this->inst[2] = inst[2];
    }
    SerialInst() {}
    SerialInst(char inst0,char inst1, char inst2) {
        this->inst[0] = inst0;
        this->inst[1] = inst1;
        this->inst[2] = inst2;
    }
};
template <typename T>
bool areArraysEq(T* arr1, T* arr2, unsigned long long len) {
    for (unsigned long long i = 0; i < len; i++) {
        if (arr2[i] != arr1[i]) return false;
    }
    return true;
}
bool operator == (const SerialInst& self, const SerialInst& other) {
    return areArraysEq<char>(self.inst,other.inst,3);
}
struct SerialInfo {
    unsigned short size;
    SerialInst inst;
    unsigned char* data;
    ~SerialInfo() {
        free(this->data);
    }
};
char *portName = nullptr;
char *ROMFileName = nullptr;
SerialInfo sread() {
    SerialInfo s;
    arduino->readSerialPort((const char*)(&(s.size)),2);
    arduino->readSerialPort((const char*)(&(s.inst)),3);
    void* d = malloc(s.size);
    s.data = (char*)d;
    arduino->readSerialPort((const char*)(s.data),s.size);
    return s;
}
namespace SerialInsts {
    SerialInst MemoryRead = SerialInst('M','R','D');
    SerialInst FatalError = SerialInst('F','E','R');
    SerialInst MemoryWrite = SerialInst('M','W','R');
    SerialInst InstructionRead = SerialInst('I','R','E');
    SerialInst ExpansionReset = SerialInst('E','R','E');
    SerialInst ExpansionResetAll = SerialInst('E','R','A');
    SerialInst ExpansionPing = SerialInst('E','P','I');
    SerialInst ExpansionAsk = SerialInst('E','A','S');
    SerialInst ExpansionRead = SerialInst('E','R','D');
    SerialInst ExpansionSend = SerialInst('E','S','E');
    SerialInst ExpansionGetID = SerialInst('E','I','D');
    SerialInst ExpansionDetect = SerialInst('E','D','E');
    SerialInst ExpansionNewRep = SerialInst('E','N','R');
    SerialInst ExpansionExists = SerialInst('E','X','E');
    SerialInst ExpansionWait = SerialInst('E','W','A');
    SerialInst MemoryGetSize = SerialInst('M','S','I');
    SerialInst Reset = SerialInst('R','S','T');
    SerialInst GetInstruction = SerialInst('G','T','I');
    SerialInst MemoryReset = SerialInst('M','R','T');
};
#define dealloc free(s.data); continuare;
void doConnection() {
    while (true) {
        SerialInfo s = sread();
        if (s.inst == SerialInsts::Reset) dealloc;
        if (s.inst == SerialInsts::GetInstruction) {
            unsigned int pc;
            char *pc_r = (char*)&pc;
            pc_r[0] = s.data[0];
            pc_r[1] = s.data[1];
            pc_r[2] = s.data[2];
            pc_r[3] = s.data[3];
            
        }
    }
};
int main(int argc, char** argv) {
    bool makingport = false;
    bool makingrom = false;
    signal(SIGINT,signalHandler);
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
    file = std::istream(ROMFileName);
    if (!file.is_open()) {
        std::cerr<< "Unable to open file." <<std::endl;
        std::cout<< "Terminated."<< std::endl;
        arduino->closeSerial();
        return -1;
    }
    doConnection();
    return 0;
}