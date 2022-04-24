#include<stdlib.h>
char getSerialByte() {
    return 0;
}
unsigned int getSerialAvailable() {
    return 0;
}
void sendSerial(char f) {}
void SerialSend(unsigned short size, char inst[3], char* f) {
    sendSerial(((char*)(&size))[0]); sendSerial(((char*)(&size))[1]);
    sendSerial(inst[0]); sendSerial(inst[1]); sendSerial(inst[2]);
    for (int i = 0; i < size; i++) sendSerial(f[i]);
}
struct SerialInfo {
    unsigned short size;
    char inst[3];
    char* data;
    SerialInfo(unsigned short size, char inst[3], char* data) {
    this->size = size;
    this->inst[0] = inst[0];
    this->inst[1] = inst[1];
    this->inst[2] = inst[2];
    this->data = data;
    }
    SerialInfo() {
        this->size = 0;
        this->data = nullptr;
    }
};
SerialInfo SerialRead() {
    while (getSerialAvailable() < 5) {};
    char a[2] = {getSerialByte(),getSerialByte()};
    char inst[3] = {getSerialByte(),getSerialByte(),getSerialByte()};
    unsigned short size = *((unsigned short*)a);
    char* data = (char*)malloc(size);
    for (int i = 0; i < size; i++) {
        while (getSerialAvailable() == 0) {}
        data[i] = getSerialByte();
    }
    return SerialInfo(size,inst,data);
}
void SerialSendNoMessage(char inst[3]) {
    SerialSend(0,inst,nullptr);
}
namespace SerialInsts {
    char MemoryRead[3] = {'M','R','D'};
    char MemoryCopy[3] = {'M','C','P'};
    char MemoryReadBulk[3] = {'M','R','B'};
    char MemoryWriteBulk[3] = {'M','W','B'};
    char FatalError[3] = {'F','E','R'};
    char MemoryWrite[3] = {'M','W','R'};
    char InstructionRead[3] = {'I','R','E'};
    char ExpansionReset[3] = {'E','R','E'};
    char ExpansionResetAll[3] = {'E','R','A'};
    char ExpansionPing[3] = {'E','P','I'};
    char ExpansionAsk[3] = {'E','A','S'};
    char ExpansionRead[3] = {'E','R','D'};
    char ExpansionSend[3] = {'E','S','E'};
    char ExpansionGetID[3] = {'E','I','D'};
    char ExpansionDetect[3] = {'E','D','E'};
    char ExpansionNewRep[3] = {'E','N','R'};
    char ExpansionExists[3] = {'E','X','E'};
    char ExpansionWait[3] = {'E','W','A'};
    char MemoryGetSize[3] = {'M','S','I'};
    char Reset[3] = {'R','S','T'};
    char GetInstruction[3] = {'G','T','I'};
    char MemoryReset[3] = {'M','R','T'};
    char Debug[3] = {'D','B','G'};
};