#include <iostream>
#include <cstdlib>
#include <csignal>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>

#include "SerialPort.hpp"
#include "types.hpp"
#include "stringUtilities.hpp"

#define or ||
#define and &&
SerialPort *arduino;
void signalHandler(int signum) {
    std::cout << "Terminated." << std::endl;
    if (arduino->isConnected()) arduino->closeSerial();
    exit(signum);
}
std::vector<unsigned char> romfile;
std::vector<std::string> configfile;
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
    arduino->writeSerialPort((const char*)(&(s.inst.inst)),3);
    arduino->writeSerialPort((const char*)s.data,s.size);
    free(s.data);
}
class MemoryC {
public:
    MemoryC();
    void initialize(unsigned int size) {
        if (this->size == 0) {
            this->size = size * 1024;
            this->ptr = malloc(size * 1024);
        }
    }
    unsigned char& operator [] (unsigned int i) {
        unsigned char f = 0;
        if (size == 0) return f;
        return ((unsigned char*)ptr)[i % this->size];
    }
    ~MemoryC() {
        free(this->ptr);
    }
    unsigned short getSize() {
        return this->size / 1024;
    }
    void reset() {
        for (unsigned int i = 0; i < this->size * 1024; i++) {
            if (i >= romfile.size()) {
                ((unsigned char*)ptr)[i] = romfile[i];
            } else {
                ((unsigned char*)ptr)[i] = 0;
            }
        }
    }
private:
    void *ptr = nullptr;
    unsigned int size = 0;
} Memory;
SerialInfo sread() {
    SerialInfo s;
    arduino->readSerialPort((const char*)(&(s.size)),2);
    arduino->readSerialPort((const char*)(&(s.inst)),3);
    void* d = malloc(s.size);
    s.data = (unsigned char*)d;
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
    const SerialInst MemoryRes = SerialInst('M','R','T');
};
#define SerialInsts__MemoryReadSW SerialInstConst('M','R','D')
#define SerialInsts__FatalErrorSW SerialInstConst('F','E','R')
#define SerialInsts__MemoryWriteSW SerialInstConst('M','W','R')
#define SerialInsts__InstructionReadSW SerialInstConst('I','R','E')
#define SerialInsts__ExpansionResetSW SerialInstConst('E','R','E')
#define SerialInsts__ExpansionResetAllSW SerialInstConst('E','R','A')
#define SerialInsts__ExpansionPingSW SerialInstConst('E','P','I')
#define SerialInsts__ExpansionAskSW SerialInstConst('E','A','S')
#define SerialInsts__ExpansionReadSW SerialInstConst('E','R','D')
#define SerialInsts__ExpansionSendSW SerialInstConst('E','S','E')
#define SerialInsts__ExpansionGetIDSW SerialInstConst('E','I','D')
#define SerialInsts__ExpansionDetectSW SerialInstConst('E','D','E')
#define SerialInsts__ExpansionNewRepSW SerialInstConst('E','N','R')
#define SerialInsts__ExpansionExistsSW SerialInstConst('E','X','E')
#define SerialInsts__ExpansionWaitSW SerialInstConst('E','W','A')
#define SerialInsts__MemoryGetSizeSW SerialInstConst('M','S','I')
#define SerialInsts__ResetSW SerialInstConst('R','S','T')
#define SerialInsts__GetInstructionSW SerialInstConst('G','T','I')
#define SerialInsts__MemoryResetSW SerialInstConst('M','R','T')
#define dealloc free(s.data); continue;
void doConnection() {
    while (true) {
        SerialInfo s = sread();
        unsigned int pc;
        char *pc_r = (char*)&pc;
        SerialInfo so;
        switch (s.inst.sw) {
            case SerialInsts__GetInstructionSW:
                pc_r[0] = s.data[0];
                pc_r[1] = s.data[1];
                pc_r[2] = s.data[2];
                pc_r[3] = s.data[3];
                so.inst = SerialInsts::GetInstruction;
                so.size = 4;
                so.data = (unsigned char*)malloc(4);
                so.data[0] = Memory[pc];
                so.data[1] = Memory[pc + 1];
                so.data[2] = Memory[pc + 2];
                so.data[3] = Memory[pc + 3];
                ssend(so);
                break;
            case SerialInsts__MemoryReadSW:
                pc_r[0] = s.data[0];
                pc_r[1] = s.data[1];
                pc_r[2] = s.data[2];
                pc_r[3] = s.data[3];
                so.inst = SerialInsts::GetInstruction;
                so.size = 1;
                so.data = (unsigned char*)malloc(1);
                so.data[0] = Memory[pc];
                ssend(so);
                break;
            case SerialInsts__MemoryWriteSW:
                pc_r[0] = s.data[0];
                pc_r[1] = s.data[1];
                pc_r[2] = s.data[2];
                pc_r[3] = s.data[3];
                Memory[pc] = s.data[4];
                break;
            case SerialInsts__MemoryGetSizeSW:
                so.size = 2;
                so.data = (unsigned char*)malloc(2);
                *((unsigned short*)(so.data)) = Memory.getSize();
                so.inst = SerialInsts::MemoryGetSize;
                ssend(so);
                break;
            case SerialInsts__ResetSW:
                Memory.reset();
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
    std::ifstream file(ROMFileName);
    if (!file.is_open()) {
        std::cerr<< "Unable to open ROM file." <<std::endl;
        std::cout<< "Terminated."<< std::endl;
        arduino->closeSerial();
        return -1;
    }
    std::string fs;
    while (!file.eof()) {
        std::string s;
        std::getline(file,s);
        fs += s + (std::string)" ";
    }
    file.close();
    fs = replace(fs,"\r"," ");
    fs = replace(fs,"\n"," ");
    for (auto i : split(fs," ")) {
        if (i.size() == 0) continue;
        romfile.push_back(std::stoi(i,nullptr,16));
    }
    file = std::ifstream(ConfigFile);
    while (!file.eof()) {
        std::string s;
        std::getline(file,s);
        fs += s;
    }
    
    file.close();
    fs = replace(fs,"\r"," ");
    fs = replace(fs,"\n"," ");
    unsigned long long arg = 0;
    enum ConfigInsts {
        NONE = 0,
        MEM_SIZE
    } currentinst = NONE;
    for (auto i : split(fs," ")) {
        if (i.size() == 0) continue;
        configfile.push_back(i);
        switch (currentinst) {
            case NONE:
                if (i == "MEM_SIZE") {
                    currentinst = MEM_SIZE;
                }
                break;
            case MEM_SIZE:
                unsigned int size = std::stoi(i,nullptr,10);
                if (size > 16384) {
                    std::cerr<< "Memory size is too large."<< std::endl;
                    std::cout<< "Terminated."<< std::endl;
                    arduino->closeSerial();
                    return -1;
                }
                Memory.initialize(size);
                currentinst = NONE;
                break;
        }
    }
    doConnection();
    return 0;
}