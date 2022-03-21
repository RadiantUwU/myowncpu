#include "SerialPort.hpp"
#include "types.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>
#define or ||
#define and &&
SerialPort *arduino;
void signalHandler(int signum) {
    std::cout << "Terminated." << std::endl;
    if (arduino->isConnected()) arduino->closeSerial();
    exit(signum);
}
std::string romfile;
std::string configfile;
unsigned int memorysize;
template <typename T>
bool areArraysEq(T* arr1, T* arr2, unsigned long long len) {
    for (unsigned long long i = 0; i < len; i++) {
        if (arr2[i] != arr1[i]) return false;
    }
    return true;
}
bool operator == (const SerialInst& self, const SerialInst& other) {
    return self.sw == other.sw;
}
bool operator == (const SerialInst& self, const unsigned int n) {
    return self.sw == n;
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
char *ConfigFile = nullptr;
void ssend(SerialInfo s) {
    if (!arduino->writeSerialPort((char*)&(s.size),2)) {
        std::cerr<< "Fatal error, arduino not detected while writing to serial." << std::endl;
        signalHandler(1);
    }
    arduino->writeSerialPort(&(s.inst),3);
    arduino->writeSerialPort(s.data,s.size);
    free(s.data);
}
class MemoryC {
public:
    MemoryC();
    void initialize(unsigned int size) {
        if (this->size == 0) {
            this->size = 0;
            this->ptr = malloc(size);
        }
    }
    unsigned char& operator [] (unsigned int i) {
        if (size == 0) return 0;
        return ((unsigned char*)ptr)[i % this->size];
    }
    ~MemoryC() {
        free(this->ptr);
    }
    unsigned short getSize() {
        return this->size / 1024;
    }
private:
    void *ptr = nullptr;
    unsigned int size = 0;
};MemoryC Memory;
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
    const SerialInst MemoryRead = SerialInst('M','R','D');
    const SerialInst FatalError = SerialInst('F','E','R');
    const SerialInst MemoryWrite = SerialInst('M','W','R');
    const SerialInst InstructionRead = SerialInst('I','R','E');
    const SerialInst ExpansionReset = SerialInst('E','R','E');
    const SerialInst ExpansionResetAll = SerialInst('E','R','A');
    const SerialInst ExpansionPing = SerialInst('E','P','I');
    const SerialInst ExpansionAsk = SerialInst('E','A','S');
    const SerialInst ExpansionRead = SerialInst('E','R','D');
    const SerialInst ExpansionSend = SerialInst('E','S','E');
    const SerialInst ExpansionGetID = SerialInst('E','I','D');
    const SerialInst ExpansionDetect = SerialInst('E','D','E');
    const SerialInst ExpansionNewRep = SerialInst('E','N','R');
    const SerialInst ExpansionExists = SerialInst('E','X','E');
    const SerialInst ExpansionWait = SerialInst('E','W','A');
    const SerialInst MemoryGetSize = SerialInst('M','S','I');
    const SerialInst Reset = SerialInst('R','S','T');
    const SerialInst GetInstruction = SerialInst('G','T','I');
    const SerialInst MemoryReset = SerialInst('M','R','T');
    unsigned int&& MemoryReadSW = SerialInst('M','R','D').sw;
    unsigned int&& FatalErrorSW = SerialInst('F','E','R').sw;
    unsigned int&& MemoryWriteSW = SerialInst('M','W','R').sw;
    unsigned int&& InstructionReadSW = SerialInst('I','R','E').sw;
    unsigned int&& ExpansionResetSW = SerialInst('E','R','E').sw;
    unsigned int&& ExpansionResetAllSW = SerialInst('E','R','A').sw;
    unsigned int&& ExpansionPingSW = SerialInst('E','P','I').sw;
    unsigned int&& ExpansionAskSW = SerialInst('E','A','S').sw;
    unsigned int&& ExpansionReadSW = SerialInst('E','R','D').sw;
    unsigned int&& ExpansionSendSW = SerialInst('E','S','E').sw;
    unsigned int&& ExpansionGetIDSW = SerialInst('E','I','D').sw;
    unsigned int&& ExpansionDetectSW = SerialInst('E','D','E').sw;
    unsigned int&& ExpansionNewRepSW = SerialInst('E','N','R').sw;
    unsigned int&& ExpansionExistsSW = SerialInst('E','X','E').sw;
    unsigned int&& ExpansionWaitSW = SerialInst('E','W','A').sw;
    unsigned int&& MemoryGetSizeSW = SerialInst('M','S','I').sw;
    unsigned int&& ResetSW = SerialInst('R','S','T').sw;
    unsigned int&& GetInstructionSW = SerialInst('G','T','I').sw;
    unsigned int&& MemoryResetSW = SerialInst('M','R','T').sw;
};
#define dealloc free(s.data); continue;
void doConnection() {
    while (true) {
        SerialInfo s = sread();
        unsigned int pc;
        char *pc_r = (char*)&pc;
        SerialInfo so;
        switch (s.sw) {
            case SerialInsts::GetInstructionSW:
                pc_r[0] = s.data[0];
                pc_r[1] = s.data[1];
                pc_r[2] = s.data[2];
                pc_r[3] = s.data[3];
                so.inst = SerialInsts::GetInstruction;
                so.size = 4;
                so.data = malloc(4);
                so.data[0] = Memory[pc];
                so.data[1] = Memory[pc + 1];
                so.data[2] = Memory[pc + 2];
                so.data[3] = Memory[pc + 3];
                ssend(so);
                break;
            case Serialinsts::MemoryReadSW:
                pc_r[0] = s.data[0];
                pc_r[1] = s.data[1];
                pc_r[2] = s.data[2];
                pc_r[3] = s.data[3];
                so.inst = SerialInsts::GetInstruction;
                so.size = 1;
                so.data = malloc(1);
                so.data[0] = Memory[pc];
                ssend(so);
                break;
            case SerialInsts::MemoryReadSW:
                pc_r[0] = s.data[0];
                pc_r[1] = s.data[1];
                pc_r[2] = s.data[2];
                pc_r[3] = s.data[3];
                Memory[pc] = s.data[4];
                break;
            case SerialInsts::MemoryGetSizeSW:
                so.size = 2;
                so.data = malloc(2);
                *((unsigned short*)(so.data)) = Memory.getSize();
                so.inst = SerialInsts::MemoryGetSize;
                ssend(so);
                break;
            
        };
        dealloc;
    }
};
int main(int argc, char** argv) {
    bool makingport = false;
    bool makingrom = false;
    bool makingconf = false;
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
                case 'c':
                    makingconf = true;
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
        } else if (makingconf) {
            ConfigFile = argv[i];
            makingconf = false;
        }
    }
    if (portName == nullptr or ROMFileName == nullptr or ConfigFile == nullptr) {
        std::cerr<< "PortName and/or RomFileName and/or ConfigFile are missing"<< std::endl;
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
    std::istream file(ROMFileName);
    if (!file.is_open()) {
        std::cerr<< "Unable to open ROM file." <<std::endl;
        std::cout<< "Terminated."<< std::endl;
        arduino->closeSerial();
        return -1;
    }
    
    doConnection();
    return 0;
}