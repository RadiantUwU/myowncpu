# 1 "c:\\Users\\Stefan\\Desktop\\Files\\my coding projects\\C++\\myowncpu\\client_build\\arduinoclient.ino"
# 2 "c:\\Users\\Stefan\\Desktop\\Files\\my coding projects\\C++\\myowncpu\\client_build\\arduinoclient.ino" 2

void memcpy(void* src, void* dest, unsigned long long size) {
    char* src_ = (char*)src;
    char* dest_ = (char*)dest;
    for (unsigned long long i = 0; i < size; i++) {
        dest_[i] = src_[i];
    }
}
template <typename T>
bool areArraysEq(T* arr1, T* arr2, unsigned long long len) {
    for (unsigned long long i = 0; i < len; i++) {
        if (arr2[i] != arr1[i]) return false;
    }
    return true;
}
inline long long positive_modulo(long long i, unsigned long long n) {
    return (i % n + n) % n;
}
namespace Config {
    enum ScreenTypes {
        _80x25 = 80*25,
        _20x4 = 20*4,
        _16x2 = 16*2,
        _8x1 = 8*1
    };
    const ScreenTypes screen_type = _20x4;
    const unsigned char screen_y = 4;
    const unsigned char screen_x = 20;
    const unsigned int keylength = 16;
    const unsigned char KOpins[4] = {8,9,10,11};
    const unsigned char KIpins[4] = {4,5,6,7};
    const unsigned char speakerpin = 3;
    const unsigned char resetpin = 2;
    const unsigned char rs = 12;
    const unsigned char en = 13;
    const unsigned char d4 = A0;
    const unsigned char d5 = A1;
    const unsigned char d6 = A2;
    const unsigned char d7 = A3;
    const unsigned char joyx = A4;
    const unsigned char joyy = A5;
    const unsigned char* keyset = (const unsigned char*)"\x01\x02\x03\x0A\x04\x05\x06\x0B\x07\x08\x09\x0C\x0E\x00\x0F\x0D";
}
char getSerialByte() {
    return Serial.read();
}
unsigned int getSerialAvailable() {
    return Serial.available();
}
void sendSerial(char f) {Serial.write(f);}
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
struct SerialInfo SerialRead() {
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
# 121 "c:\\Users\\Stefan\\Desktop\\Files\\my coding projects\\C++\\myowncpu\\client_build\\arduinoclient.ino"
void printDebug(const char* f) {}
void printDebug(char* f, size_t n) {}

const 
# 124 "c:\\Users\\Stefan\\Desktop\\Files\\my coding projects\\C++\\myowncpu\\client_build\\arduinoclient.ino" 3
     __attribute__((__progmem__)) 
# 124 "c:\\Users\\Stefan\\Desktop\\Files\\my coding projects\\C++\\myowncpu\\client_build\\arduinoclient.ino"
             char hexv[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
void hexPrint(char* p, unsigned long long n, size_t b) {
    for (int i = b; i >= 0; i--) {
        p[i] = hexv[(n >> ((b - i) * 4)) % 16];
    }
}
LiquidCrystal lcd(Config::rs, Config::en, Config::d4, Config::d5, Config::d6, Config::d7);
class Emulator;
void ExpansionHandle(SerialInfo s);
void hardreset() {
  analogWrite(Config::resetpin, -1);
  analogWrite(Config::resetpin, 255);
};//tomodify
void halt() {while (1) {}};


class Memory {
public:
    void write(unsigned int in, unsigned char byte) {
        if (!infastmode) {
            char* data = (char*)malloc(5);
            char* ind = (char*)(&in);
            memcpy(ind,data,4);
            data[4] = byte;
            SerialSend(5,SerialInsts::MemoryWrite,data);
            free(data);
        } else {
            fastmem[in % 512] = byte;
        }
    }
    unsigned char read(unsigned int in) {
        char* data = (char*)malloc(4);
        char* ind = (char*)(&in);
        memcpy(ind,data,4);
        SerialSend(4,SerialInsts::MemoryRead,data);
        free(data);
        while (1) {
            SerialInfo s = SerialRead();
            if (!areArraysEq(s.inst,SerialInsts::MemoryRead,3)) ExpansionHandle(s);
            else {
                unsigned char d = s.data[0];
                free(s.data);
                return d;
            }
        }
    }
    unsigned short getSize() {
        SerialSendNoMessage(SerialInsts::MemoryGetSize);
        while (1) {
            SerialInfo s = SerialRead();
            if (!areArraysEq(s.inst,SerialInsts::MemoryGetSize,3)) ExpansionHandle(s);
            else {
                unsigned short d = s.data[0] + s.data[1] * 256;
                free(s.data);
                return d;
            }
        }
    }
    unsigned char* getInst(unsigned int in) {
        if (!infastmode) {
            char* ind = (char*)(&in);
            char* data = (char*)malloc(4);
            memcpy(ind,data,4);
            SerialSend(4,SerialInsts::GetInstruction,data);
            free(data);
            SerialInfo s;
            while (1) {
                s = SerialRead();



                if (!areArraysEq(s.inst,SerialInsts::GetInstruction,3)) ExpansionHandle(s);
                else break;
            }
            return (unsigned char*)s.data;
        } else {
            unsigned char* data = (unsigned char*)malloc(4);
            if ((in % 512) > (512 - 4)) {
                data[0] = fastmem[(in + 0) % 512];
                data[1] = fastmem[(in + 1) % 512];
                data[2] = fastmem[(in + 2) % 512];
                data[3] = fastmem[(in + 3) % 512];
            } else {
                memcpy(fastmem + (in % 512),data,4);
            }
            return data;
        }
    }
    bool infastmode = false;
    bool initfast() {
        if (fastmem == nullptr) {
            fastmem = (unsigned char*)malloc(512);
            return fastmem != nullptr;
        } else return true;
    }
    void uninitfast() {
        if (fastmem != nullptr) {
          free(fastmem);
          fastmem = nullptr;
        }
    }
    bool fastinitialized() {return fastmem != nullptr;}
    bool copytofast = false;
    bool copyfromfast = false;
    unsigned int memadrsource = 0;
    unsigned int memadrdest = 0;
    unsigned int memcopysize = 0;
    void memcopy() {
        if (!fastinitialized()) {
            unsigned int* v = (unsigned int*)malloc(3);
            v[0] = memadrsource;
            v[1] = memadrdest;
            v[2] = memcopysize;
            SerialSend(12,SerialInsts::MemoryCopy,(char*)v);
            free(v);
        } else switch(copytofast * 2 + copyfromfast) {
            case 0:
                if (true) {
                    unsigned int* v = (unsigned int*)malloc(12);
                    v[0] = memadrsource;
                    v[1] = memadrdest;
                    v[2] = memcopysize;
                    SerialSend(12,SerialInsts::MemoryCopy,(char*)v);
                    free(v);
                }
                break;
            case 1:
                if (true) {
                    unsigned int* v = (unsigned int*)malloc(8);
                    v[0] = memadrsource;
                    v[1] = memcopysize;
                    SerialSend(8,SerialInsts::MemoryReadBulk,(char*)v);
                    free(v);
                    SerialInfo s = SerialRead();
                    if (!areArraysEq(s.inst,SerialInsts::MemoryReadBulk,3)) ExpansionHandle(s);
                    else {
                        for (unsigned int i = 0; i < memcopysize; i++) {
                            fastmem[(memadrdest + i) % 512] = Serial.read();
                        }
                    }
                    free(s.data);
                }
                break;
            case 2:
                if (true) {
                    unsigned int* v = (unsigned int*)malloc(8);
                    v[0] = memadrdest;
                    v[1] = memcopysize;
                    SerialSend(8 + memcopysize,SerialInsts::MemoryWriteBulk,(char*)v);
                    free(v);
                    for (int i = 0; i < memcopysize; i++) Serial.write(fastmem[(memadrsource + i) % 512]);
                }
                break;
            case 3:
                for (int i = 0; i < memcopysize; i++) fastmem[(memadrdest + i) % 512] = fastmem[(memadrsource + i) % 512];
                break;
        }
    }
protected:
    unsigned char* fastmem = nullptr;
};
struct KeyEvent {
    unsigned char key = 0;
    bool isdown = 0;
    KeyEvent(unsigned char key, bool isdown) {this->key = key; this->isdown = isdown;}
    KeyEvent() {}
};
class KeyEventQueue {
public:
    KeyEventQueue() {};
    KeyEvent events[64];
    bool keys[16];
    bool enabled = false;
    unsigned char p;
    unsigned char k;
    bool* getkeystates() {
        bool* f = (bool*)malloc(sizeof(bool) * 16);
        for (int c = 0; c < 4; c++) {
            digitalWrite(Config::KIpins[c], 0);
            for (int r = 0; r < 4; r++) if (!digitalRead(Config::KOpins[r])) f[Config::keyset[r * 4 + c]] = 1;
            digitalWrite(Config::KIpins[c], 1);
        }
        return f;
    }
    void push(KeyEvent k) {
        this->events[this->p] = k;
        this->p++;
        this->p%=64;
    }
    KeyEvent pull() {
        KeyEvent k = this->events[0];
        this->p--;
        for (int i = 1; i < 64; i++) this->events[i - 1] = this->events[i];
        return k;
    }
    bool isAnyKeyDown() {
        return ((this->keys[0] || this->keys[1]) || (this->keys[2] || this->keys[3])) || ((this->keys[4] || this->keys[5]) || (this->keys[6] || this->keys[7]));
    }
};
class StackRegister {
public:
    unsigned char stackpointer;
    unsigned char stack[256];
    void push(unsigned char val) {
        this->stack[this->stackpointer] = val;
        this->stackpointer++;
    }
    unsigned char pop() {
        this-stackpointer--;
        return this->stack[this->stackpointer];
    }
    void pushaddy(unsigned char lo, unsigned char mi, unsigned char hi) {
        this->push(lo);
        this->push(mi);
        this->push(hi);
    }
    unsigned char * getaddy() {
        unsigned char* g = (unsigned char*)malloc(3);
        g[2] = this->pop();
        g[1] = this->pop();
        g[0] = this->pop();
        return g;
    }
    Emulator* emu;
    StackRegister(Emulator* emu) {this->emu = emu;}
    bool condeval(unsigned char condition);
};
enum ALUOperations {
    ADD,SUB,
    MUL,MOD,DIV,
    SHL,SHR,
    AND,OR,XOR,
    NAND,NOR,XNOR,
    NOT
};

class ALU {
public:
    Emulator* emu;
    ALU(Emulator* emu) {this->emu = emu;}
    bool overflow = false;
    unsigned char OP(ALUOperations op, unsigned char A, unsigned char B) {
        switch (op) {
            case ADD:
                this->overflow = (int)(A + B) > 255;
                return positive_modulo((A + B),256);
            case SUB:
                this->overflow = (int)(A - B) < 0;
                return positive_modulo((A - B),256);
            case MUL:
                this->overflow = (int)(A * B) > 255;
                return positive_modulo((A * B),256);
            case MOD:
                this->overflow = B == 0;
                if (B == 0) {
                    return 0;
                }
                return A % B;
            case DIV:
                this->overflow = B == 0;
                if (B == 0) {
                    return 0;
                }
                return (unsigned char)(A / B);
            case SHL:
                this->overflow = A > 128;
                return (unsigned char)(A << 1);
            case SHR:
                this->overflow = (A % 2) == 1;
                return (unsigned char)(A >> 1);
            case AND:
                this->overflow = false;
                return (A & B);
            case OR:
                this->overflow = false;
                return (A | B);
            case XOR:
                this->overflow = false;
                return (A ^ B);
            case NAND:
                this->overflow = false;
                return ~(A & B);
            case NOR:
                this->overflow = false;
                return ~(A | B);
            case XNOR:
                this->overflow = false;
                return ~(A ^ B);
            case NOT:
                this->overflow = false;
                return ~A;
        }
    }
};
struct Interrupt {
    bool enabled = false;
    unsigned char condition = 0;
    unsigned char address[3] = {0,0,0};
    bool evaluate();
};
class Joystick {
public:
    static unsigned char xpos() {
        return analogRead(Config::joyx) * 3 / 1024;
    }
    static unsigned char ypos() {
        return analogRead(Config::joyy) * 3 / 1024;
    }
};
class ExpansionInterface;
ExpansionInterface* exi = nullptr;
class ExpansionInterface {
public:
    ExpansionInterface(Emulator* emu) {
        exi = this;
        this->emu = emu;
    }
    Emulator* emu;
    unsigned short expadr;
    unsigned char read() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcpy(ind,data,2);
        SerialSend(2,SerialInsts::ExpansionRead,data);
        free(data);
        while (1) {
            SerialInfo s = SerialRead();
            if (!areArraysEq(s.inst,SerialInsts::ExpansionRead,3)) ExpansionHandle(s);
            else {
                unsigned char d = s.data[0];
                free(s.data);
                return d;
            }
        }
    }
    bool newReply() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcpy(ind,data,2);
        SerialSend(2,SerialInsts::ExpansionNewRep,data);
        free(data);
        while (1) {
            SerialInfo s = SerialRead();
            if (!areArraysEq(s.inst,SerialInsts::ExpansionNewRep,3)) ExpansionHandle(s);
            else {
                bool d = s.data[0] == 1;
                free(s.data);
                return d;
            }
        }
    }
    bool exists() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcpy(ind,data,2);
        SerialSend(2,SerialInsts::ExpansionExists,data);
        free(data);
        while (1) {
            SerialInfo s = SerialRead();
            if (!areArraysEq(s.inst,SerialInsts::ExpansionExists,3)) ExpansionHandle(s);
            else {
                bool d = s.data[0] == 1;
                free(s.data);
                return d;
            }
        }
    }
    bool ping() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcpy(ind,data,2);
        SerialSend(2,SerialInsts::ExpansionPing,data);
        free(data);
        while (1) {
            SerialInfo s = SerialRead();
            if (!areArraysEq(s.inst,SerialInsts::ExpansionPing,3)) ExpansionHandle(s);
            else {
                bool d = s.data[0] == 1;
                free(s.data);
                return d;
            }
        }
    }
    void send(unsigned char data_) {
        char* data = (char*)malloc(3);
        char* ind = (char*)(&(this->expadr));
        memcpy(ind,data,2);
        data[2] = data_;
        SerialSend(3,SerialInsts::ExpansionRead,data);
        free(data);
    }
    void reset() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcpy(ind,data,2);
        SerialSend(2,SerialInsts::ExpansionRead,data);
        free(data);
    }
    unsigned short getID() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcpy(ind,data,2);
        SerialSend(2,SerialInsts::ExpansionGetID,data);
        free(data);
        while (1) {
            SerialInfo s = SerialRead();
            if (!areArraysEq(s.inst,SerialInsts::ExpansionGetID,3)) ExpansionHandle(s);
            else {
                unsigned short d = *(unsigned short*)(s.data);
                free(s.data);
                return d;
            }
        }
    }
    void resetAll() {
        SerialSendNoMessage(SerialInsts::ExpansionResetAll);
    }
    unsigned char ask() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcpy(ind,data,2);
        SerialSend(2,SerialInsts::ExpansionAsk,data);
        free(data);
        while (1) {
            SerialInfo s = SerialRead();
            if (!areArraysEq(s.inst,SerialInsts::ExpansionAsk,3)) ExpansionHandle(s);
            else {
                unsigned char d = s.data[0];
                free(s.data);
                return d;
            }
        }
    }
    unsigned char wait() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcpy(ind,data,2);
        SerialSend(2,SerialInsts::ExpansionWait,data);
        free(data);
        while (1) {
            SerialInfo s = SerialRead();
            if (!areArraysEq(s.inst,SerialInsts::ExpansionWait,3)) ExpansionHandle(s);
            else {
                unsigned char d = s.data[0];
                free(s.data);
                return d;
            }
        }
    }
};
void ExpansionHandle(SerialInfo s) {
    if (areArraysEq(s.inst,SerialInsts::Reset,3)) hardreset();
    //not supposed to get triggered under any way
    SerialSendNoMessage(SerialInsts::FatalError);
    halt();
}
class Terminal {
public:
    unsigned char memory[(unsigned short)Config::screen_type] = {0};
    static const unsigned short size = Config::screen_type;
    unsigned char x = 0;
    unsigned char y = 0;
    bool autoincrement = 0;
    Terminal() {}
    void write(unsigned short place, unsigned char val) {
        if (place % 4 != 0) return;
        if (place / 4 > this->size) return;
        this->memory[(unsigned short)(place / 4)] = val;
    }
    unsigned char read(unsigned short place) {
        if (place % 4 != 0) return 0;
        if (place / 4 > this->size) return 0;
        return this->memory[(unsigned short)(place / 4)];
    }
    void put(unsigned char character) {
        this->write(this->x * 4 + this->y * 4 * Config::screen_x,character);
        if (this->autoincrement) {
            this->x++;
            if (this->x == Config::screen_x) {
                this->x = 0;
                this->y++;
                if (this->y == Config::screen_y) this->y = 0;
            }
        }
    }
    void refresh() {
        lcd.clear();
        for (int y = 0; y < Config::screen_y; y++) {
            lcd.setCursor(0,y);
            for (int x = 0; x < Config::screen_x; x++) {
# 633 "c:\\Users\\Stefan\\Desktop\\Files\\my coding projects\\C++\\myowncpu\\client_build\\arduinoclient.ino"
                unsigned char c = this->memory[x + y * Config::screen_x];
                if (c != 0 && c != 32) lcd.write(c);
                else lcd.setCursor(x + 1,y);
            }
        }
    };//tomodify
};
struct TimerInfo {
    unsigned short inc = 0;
    unsigned short dec = 0;
    TimerInfo() {}
};
# 654 "c:\\Users\\Stefan\\Desktop\\Files\\my coding projects\\C++\\myowncpu\\client_build\\arduinoclient.ino"
class Emulator {
public:
    ALU* a;
    Memory* m;
    StackRegister* s;
    ExpansionInterface* e;
    Interrupt i[2];
    Terminal* t;
    KeyEventQueue* k;
    Joystick* j;
    Emulator() {
        SerialSendNoMessage(SerialInsts::Reset);
    }
    unsigned char registers[64];
    TimerInfo tt[64];
    bool ttovfw = false;
    unsigned char rega = 0;
    unsigned char regb = 0;
    unsigned char ret[3] = {0,0,0};
    unsigned int pc = 0;
    unsigned char jumpcondition = 0;
    bool memautoinc = 0;
    long time = 0;
    long ct = 0;
    bool freqlocked = 0;
    bool willexecfast = 0;
    bool memexecfast = 0;
    bool memadrfast = 0;
    unsigned int cfr = 1;
    unsigned char ininterrupt = 0;
    bool runinst(unsigned char inst, unsigned char arg1, unsigned char arg2, unsigned char arg3) {
# 701 "c:\\Users\\Stefan\\Desktop\\Files\\my coding projects\\C++\\myowncpu\\client_build\\arduinoclient.ino"
        switch (inst) {
            case 0:
                return true;
            case 1:
                this->pc += 1;
                break;
            case 2:
                hardreset();
                return true;
            case 3:
                this->i[0].enabled = true;
                this->pc += 1;
                break;
            case 4:
                this->i[0].enabled = false;
                this->pc += 1;
                break;
            case 5:
                this->pc += 1;
                this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                this->pc = this->i[0].address[0] + this->i[0].address[1] * 256 + this->i[0].address[2] * 65536;
                this->ininterrupt++;
                break;
            case 6:
                this->i[0].condition = arg1;
                this->pc += 2;
                break;
            case 7:
                this->i[0].address[0] = arg1;
                this->pc += 2;
                break;
            case 8:
                this->i[0].address[1] = arg1;
                this->pc += 2;
                break;
            case 9:
                this->i[0].address[2] = arg1;
                this->pc += 2;
                break;
            case 10:
                this->i[0].address[0] = arg1;
                this->i[0].address[1] = arg2;
                this->i[0].address[2] = arg3;
                this->pc += 4;
                break;
            case 11:
                this->i[1].enabled = true;
                this->pc += 1;
                break;
            case 12:
                this->i[1].enabled = false;
                this->pc += 1;
                break;
            case 13:
                this->pc += 1;
                this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                this->pc = this->i[1].address[0] + this->i[1].address[1] * 256 + this->i[1].address[2] * 65536;
                this->ininterrupt++;
                break;
            case 14:
                this->i[1].condition = arg1;
                this->pc += 2;
                break;
            case 15:
                this->i[1].address[0] = arg1;
                this->pc += 2;
                break;
            case 16:
                this->i[1].address[1] = arg1;
                this->pc += 2;
                break;
            case 17:
                this->i[1].address[2] = arg1;
                this->pc += 2;
                break;
            case 18:
                this->i[1].address[0] = arg1;
                this->i[1].address[1] = arg2;
                this->i[1].address[2] = arg3;
                this->pc += 4;
                break;
            case 19:
                this->memaddr[0] = arg1;
                this->pc += 2;
                break;
            case 20:
                this->memaddr[1] = arg1;
                this->pc += 2;
                break;
            case 21:
                this->memaddr[2] = arg1;
                this->pc += 2;
                break;
            case 22:
                this->memaddr[0] = arg1;
                this->memaddr[1] = arg2;
                this->memaddr[2] = arg3;
                this->pc += 4;
                break;
            case 23:
                this->memaddr[0]++;
                if (this->memaddr[0] == 0) {
                    this->memovfw = true;
                    this->memaddr[1]++;
                    if (this->memaddr[1] == 0) this->memaddr[2]++;
                } else this->memovfw = false;
                this->pc += 1;
                break;
            case 24:
                this->memaddr[1]++;
                if (this->memaddr[1] == 0) {
                    this->memovfw = true;
                    this->memaddr[2]++;
                } else this->memovfw = false;
                this->pc += 1;
                break;
            case 25:
                this->memaddr[2]++;
                this->memovfw = this->memaddr[2] == 0;
                this->pc += 1;
                break;
            case 26:
                this->memaddr[0]--;
                if (this->memaddr[0] == 255) {
                    this->memovfw = true;
                    this->memaddr[1]--;
                    if (this->memaddr[1] == 255) this->memaddr[2]--;
                } else this->memovfw = false;
                this->pc += 1;
                break;
            case 27:
                this->memaddr[1]--;
                if (this->memaddr[1] == 255) {
                    this->memovfw = true;
                    this->memaddr[2]--;
                } else this->memovfw = false;
                this->pc += 1;
                break;
            case 28:
                this->memaddr[2]--;
                this->memovfw = this->memaddr[2] == 255;
                this->pc += 1;
                break;
            case 29:
                if (this->m->fastinitialized()) this->m->infastmode = memadrfast;
                else this->m->infastmode = false;
                this->registers[arg1 % 64] = this->m->read(this->memaddr[0] + this->memaddr[1] * 256 + this->memaddr[2] * 65536);
                if (this->memautoinc) {
                    this->memaddr[0]++;
                    if (this->memaddr[0] == 0) {
                        this->memovfw = true;
                        this->memaddr[1]++;
                        if (this->memaddr[1] == 0) this->memaddr[2]++;
                    } else this->memovfw = false;
                }
                this->pc += 2;
                break;
            case 30:
                if (this->m->fastinitialized()) this->m->infastmode = memadrfast;
                else this->m->infastmode = false;
                this->m->write(this->memaddr[0] + this->memaddr[1] * 256 + this->memaddr[2] * 65536,this->registers[arg1 % 64]);
                if (this->memautoinc) {
                    this->memaddr[0]++;
                    if (this->memaddr[0] == 0) {
                        this->memovfw = true;
                        this->memaddr[1]++;
                        if (this->memaddr[1] == 0) this->memaddr[2]++;
                    } else this->memovfw = false;
                }
                this->pc += 2;
                break;
            case 31:
                this->registers[arg1 % 64] = arg2;
                this->pc += 3;
                break;
            case 32:
                this->registers[arg1 % 64]++;
                this->pc += 2;
                this->memovfw = this->registers[arg1 % 64] == 0;
                break;
            case 33:
                this->registers[arg1 % 64]--;
                this->pc += 2;
                this->memovfw = this->registers[arg1 % 64] == 255;
                break;
            case 34:
                this->memautoinc = true;
                this->pc += 1;
                break;
            case 35:
                this->memautoinc = false;
                this->pc += 1;
                break;
            case 36:
                if (this->m->fastinitialized()) this->m->infastmode = memadrfast;
                else this->m->infastmode = false;
                if (1) {unsigned short s = this->m->getSize();
                this->registers[arg1 % 64] = s % 256;
                this->registers[arg2 % 64] = s >> 8;}
                this->pc += 3;
                break;
            case 37:
                this->registers[arg1 % 64] = this->memaddr[0];
                this->pc += 2;
                break;
            case 38:
                this->registers[arg1 % 64] = this->memaddr[1];
                this->pc += 2;
                break;
            case 39:
                this->registers[arg1 % 64] = this->memaddr[2];
                this->pc += 2;
                break;
            case 40:
                this->registers[arg2 % 64] = this->registers[arg1 % 64];
                this->pc += 3;
                break;
            case 41:
                this->registers[arg2 % 64] = this->registers[this->registers[arg1 % 64] % 64];
                this->pc += 3;
                break;
            case 42:
                this->registers[arg1 % 64]++;
                this->pc += 2;
                this->a->overflow = this->registers[arg1 % 64] == 0;
                break;
            case 43:
                this->registers[arg1 % 64]--;
                this->pc += 2;
                this->a->overflow = this->registers[arg1 % 64] == 255;
                break;
            case 44:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(ADD,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 45:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(SUB,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 46:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(MUL,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 47:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(MOD,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 48:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(DIV,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 49:
                this->pc += 3; this->rega = arg1; this->registers[arg2 % 64] = this->a->OP(SHL,this->registers[arg1 % 64],0);
                break;
            case 50:
                this->pc += 3; this->rega = arg1; this->registers[arg2 % 64] = this->a->OP(SHR,this->registers[arg1 % 64],0);
                break;
            case 51:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(AND,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 52:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(OR,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 53:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(XOR,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 54:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(NAND,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 55:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(NOR,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 56:
                this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP(XNOR,this->registers[arg1 % 64],this->registers[arg2 % 64]);
                break;
            case 57:
                this->pc += 3;
                this->rega = arg1;
                this->regb = arg2;
                break;
            case 58:
                this->pc += 3; this->rega = arg1; this->registers[arg2 % 64] = this->a->OP(SHR,this->registers[arg1 % 64],0);
                break;
            case 59:
                this->jumpcondition = arg1;
                this->pc += 2;
                break;
            case 60:
                this->pc = arg1 + arg2 * 256 + arg3 * 65536;
                break;
            case 61:
                if ((this->s->condeval(this->jumpcondition))) this->pc = arg1 + arg2 * 256 + arg3 * 65536;
                else this->pc += 4;
                break;
            case 62:
                this->pc += 4;
                this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                this->pc = arg1 + arg2 * 256 + arg3 * 65536;
                if (this->ininterrupt != 0) this->ininterrupt++;
                break;
            case 63:
                this->pc += 4;
                if ((this->s->condeval(this->jumpcondition))) {
                    this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                    this->pc = arg1 + arg2 * 256 + arg3 * 65536;
                    if (this->ininterrupt != 0) this->ininterrupt++;
                }
                break;
            case 64:
                if (!(this->s->condeval(this->jumpcondition))) this->pc = arg1 + arg2 * 256 + arg3 * 65536;
                else this->pc += 4;
                break;
            case 65:
                this->pc += 4;
                if (!(this->s->condeval(this->jumpcondition))) {
                    this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                    this->pc = arg1 + arg2 * 256 + arg3 * 65536;
                    if (this->ininterrupt != 0) this->ininterrupt++;
                }
                break;
            case 66:
                this->ret[0] = 255;
                this->ret[1] = 255;
                this->ret[2] = 255;
                if (1) {unsigned char* a = this->s->getaddy();
                this->pc = a[0] + a[1] * 256 + a[2] * 65536;
                free(a);}
                if (this->ininterrupt != 0) this->ininterrupt--;
                break;
            case 67:
                this->ret[0] = arg1;
                this->pc += 2;
                break;
            case 68:
                this->ret[1] = arg1;
                this->pc += 2;
                break;
            case 69:
                this->ret[2] = arg1;
                this->pc += 2;
                break;
            case 70:
                this->registers[arg1 % 64] = this->ret[0];
                this->pc += 2;
                break;
            case 71:
                this->registers[arg1 % 64] = this->ret[1];
                this->pc += 2;
                break;
            case 72:
                this->registers[arg1 % 64] = this->ret[2];
                this->pc += 2;
                break;
            case 73:
                this->ret[0] = arg1;
                this->ret[1] = arg2;
                this->ret[2] = arg3;
                if (1) {unsigned char* a = this->s->getaddy();
                this->pc = a[0] + a[1] * 256 + a[2] * 65536;
                free(a);}
                if (this->ininterrupt != 0) this->ininterrupt--;
                break;
            case 74:
                this->s->push(this->registers[arg1 % 64]);
                this->pc += 2;
                break;
            case 75:
                this->registers[arg1 % 64] = this->s->pop();
                this->pc += 2;
                break;
            case 76:
                this->s->stackpointer = this->registers[arg1 % 64];
                this->pc += 2;
                break;
            case 77:
                this->registers[arg1 % 64] = this->s->stackpointer;
                this->pc += 2;
                break;
            case 78:
                if (true) {
                    KeyEvent k = this->k->pull();
                    this->registers[arg1 % 64] = k.key;
                    this->registers[arg2 % 64] = (unsigned int)k.isdown * 128;
                }
                this->pc += 3;
                break;
            case 79:
                this->k->enabled = true;
                this->pc += 1;
                break;
            case 80:
                this->k->enabled = false;
                this->pc += 1;
                break;
            case 81:
                this->k->k = this->registers[arg1 % 64];
                this->pc += 3;
                break;;
            case 82:
                this->k->p = 0;
                this->pc += 1;
                break;
            case 83:
                for (int i = 0; i < this->t->size; i++) {
                    this->t->memory[i] = 0;
                }
                this->pc += 1;
                break;
            case 84:
                this->t->refresh();
                this->pc += 1;
                break;
            case 85:
                this->t->put(this->registers[arg1 % 64]);
                this->pc += 2;
                break;
            case 86:
                for (int i = 0; i < this->t->size; i++) {
                    this->t->memory[i] = this->registers[arg1 % 64];
                }
                this->pc += 2;
                break;
            case 87:
                for (int i = 0; i < this->t->size; i++) {
                    this->t->memory[i] = 0;
                }
                this->t->x = 0;
                this->t->y = 0;
                this->t->autoincrement = false;
                this->t->refresh();
                this->pc += 1;
                break;
            case 88:
                this->t->x = this->registers[arg1 % 64];
                this->pc += 2;
                break;
            case 89:
                this->t->y = this->registers[arg1 % 64];
                this->pc += 2;
                break;
            case 90:
                this->t->x = this->registers[arg1 % 64];
                this->t->y = this->registers[arg2 % 64];
                this->pc += 3;
                break;
            case 91:
                this->t->write(this->registers[arg1 % 64] + this->registers[arg2 % 64] * 256,this->registers[arg3 % 64]);
                this->pc += 4;
                break;
            case 92:
                this->pc += 3;
                break;
            case 93:
                this->pc += 2;
                break;
            case 94:
                this->pc += 2;
                break;
            case 95:
                this->tt[arg1 % 64].inc += this->registers[arg2 % 64] + 1;
                this->pc += 3;
                break;
            case 96:
                this->tt[arg1 % 64].dec += this->registers[arg2 % 64] + 1;
                this->pc += 3;
                break;
            case 97:
                this->tt[arg1 % 64].inc = 0;
                this->tt[arg1 % 64].dec = 0;
                this->pc += 2;
                break;
            case 98:
                this->ttovfw = false;
                this->pc += 1;
                break;
            case 99:
                this->registers[arg3 % 64] = this->t->read(this->registers[arg1 % 64] + this->registers[arg2 % 64] * 256);
                this->pc += 4;
                break;
            case 100:
                this->registers[arg1 % 64] = 0;
                this->pc += 2;
                break;
            case 101:
                this->registers[arg1 % 64] = 1;
                this->pc += 2;
                break;
            case 102:
                this->e->expadr = this->registers[arg1 % 64];
                this->e-> reset();
                this->pc += 2;
                break;
            case 103:
                this->e->expadr = this->registers[arg1 % 64];
                this->registers[arg2 % 64] = this->e-> ask();
                this->pc += 3;
                break;
            case 104:
                this->e->expadr = this->registers[arg1 % 64];
                this->registers[arg2 % 64] = this->e-> read();
                this->pc += 3;
                break;
            case 105:
                this->e->expadr = this->registers[arg1 % 64];
                this->e-> send(this->registers[arg2 % 64]);
                this->pc += 3;
                break;
            case 106:
                this->e->expadr = this->registers[arg1 % 64];
                this->registers[arg2 % 64] = this->e-> wait();
                this->pc += 3;
                break;
            case 107:
                this->e-> resetAll();
                this->pc += 1;
                break;
            case 108:
                this->e->expadr = this->registers[arg1 % 64];
                if (true) {
                    unsigned short s = this->e-> getID();
                    this->registers[arg2 % 64] = s % 256;
                    this->registers[arg3 % 64] = s >> 8;
                }
                this->pc += 4;
                break;
            case 109:
                this->e->expadr = this->registers[arg1 % 64];
                this->registers[arg2 % 64] = this->e-> ping();
                this->pc += 3;
                break;
            case 110:
                this->e->expadr = this->registers[arg1 % 64];
                this->pc += 2;
                break;
            case 111:
                this->freqlocked = true;
                this->cfr = arg1 + 1 + arg2 * 256;
                this->pc += 3;
                break;
            case 112:
                this->freqlocked = false;
                this->pc += 1;
                break;
            case 113:
                this->t->autoincrement = true;
                this->pc += 1;
                break;
            case 114:
                this->t->autoincrement = false;
                this->pc += 1;
                break;
            case 115:
                this->registers[arg1 % 64] = this->j->xpos();
                this->pc += 2;
                break;
            case 116:
                this->registers[arg1 % 64] = this->j->ypos();
                this->pc += 2;
                break;
            case 117:
                tone(Config::speakerpin,this->registers[arg1 % 64] + this->registers[arg2 % 64] * 256);
                this->pc += 3;
                break;
            case 118:
                noTone(Config::speakerpin);
                this->pc += 1;
                break;
            case 119:
                this->pc += 4;
                this->s->pushaddy(this->registers[arg1 % 64], this->registers[arg2 % 64], this->registers[arg3 % 64]);
                break;
            case 120:
                this->pc += 4;
                if (true) {
                    unsigned char *f = this->s->getaddy();
                    this->registers[arg1 % 64] = f[0];
                    this->registers[arg2 % 64] = f[1];
                    this->registers[arg3 % 64] = f[2];
                    free(f);
                }
                break;
            case 121:
                this->pc += 1;
                if (this->m->fastinitialized()) {
                    this->willexecfast = !this->memexecfast;
                }
                break;
            case 122:
                this->pc += 1;
                this->willexecfast = this->memexecfast;
                break;
            case 123:
                this->pc += 1;
                if(this->m->fastinitialized()) {
                    this->memadrfast = !this->memadrfast;
                }
                break;
            case 124:
                this->pc += 1;
                this->m->initfast();
                break;
            case 125:
                this->pc += 1;
                this->m->uninitfast();
                this->memexecfast = 0;
                this->memadrfast = 0;
                this->willexecfast = 0;
                break;
            case 126:
                this->pc += 1;
                this->m->memadrsource = this->memaddr[0] + this->memaddr[1] * 256 + this->memaddr[2] * 65536;
                break;
            case 127:
                this->pc += 1;
                this->m->memadrdest = this->memaddr[0] + this->memaddr[1] * 256 + this->memaddr[2] * 65536;
                break;
            case 128:
                this->pc += 3;
                this->m->copyfromfast = arg1 % 2;
                this->m->copytofast = arg2 % 2;
                break;
            case 129:
                this->pc += 4;
                this->m->memcopysize = arg1 + 1 + arg2 * 256 + arg3 * 65536;
                break;
            case 130:
                this->pc += 1;
                this->m->memcopy();
                break;
            default:
                return true;
        }
        return false;
    };
    void interruptcheck() {
        if (this->ininterrupt != 0) return;
        if (this->i[0].evaluate()) {
            this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
            this->pc = this->i[0].address[0] + this->i[0].address[1] * 256 + this->i[0].address[2] * 65536;
            this->ininterrupt++;
            return;
        }
        if (this->i[1].evaluate()) {
            this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
            this->pc = this->i[1].address[0] + this->i[1].address[1] * 256 + this->i[1].address[2] * 65536;
            this->ininterrupt++;
            return;
        }
    }
    void timerregistercycle() {
        if (millis() - this->time <= 10) return;
        this->time = millis();
        for (auto i = 0; i < 64; i++) {
            TimerInfo& t = this->tt[i];
            unsigned char& r = this->registers[i];
            if (t.inc > 0) {r++;t.inc--;}
            if (t.dec > 0) {r--;t.dec--;}
        }
    }
    void clockfreq() {
        if (!freqlocked) return;
        delay((long double)1 / (long double)cfr * 1000 - (long double)(millis() - this->ct));
        this->ct = millis();
    }
    void start() {
        for (auto i = 0; i < 64; i++) this->registers[i] = 0;
        while(1) {
            this->timerregistercycle();
            this->interruptcheck();
            if (this->m->fastinitialized()) this->m->infastmode = memexecfast;
            else this->m->infastmode = false;
            unsigned char* data = this->m->getInst(this->pc);
            unsigned char inst[4];
            memcpy(data,inst,4);
            free(data);
            if (runinst(inst[0],inst[1],inst[2],inst[3])) halt();
        }
    };
    bool memovfw = false;
    unsigned char memaddr[3] = {0,0,0};
};
bool Interrupt::evaluate() {
    if (!this->enabled) return false;
    Emulator* e = exi->emu;
    return e->s->condeval(this->condition);
}
bool StackRegister::condeval(unsigned char cond) {
    switch (cond) {
        case 1:
            return true;
        case 2:
            return this->emu->registers[this->emu->rega] % 2 == 1;
        case 3:
            return this->emu->registers[this->emu->rega] > 127;
        case 4:
            return this->emu->registers[this->emu->rega] > this->emu->registers[this->emu->regb];
        case 5:
            return this->emu->registers[this->emu->rega] == this->emu->registers[this->emu->regb];
        case 6:
            return this->emu->registers[this->emu->rega] < this->emu->registers[this->emu->regb];
        case 7:
            return this->emu->memovfw;
        case 8:
            return this->emu->a->overflow;
        case 9:
            return this->emu->e->exists();
        case 10:
            return this->emu->e->newReply();
        case 11:
            return this->emu->k->p != 0;
        case 12:
            return this->stackpointer == 0;
        case 13:
            return this->emu->ttovfw;
        case 14:
            return this->emu->registers[this->emu->rega] == 0;
        case 15:
            return this->emu->k->keys[this->emu->k->k % 16];
        case 16:
            return this->emu->k->isAnyKeyDown();
        case 17:
            return true;
        case 18:
            return true;
        case 19:
            return this->emu->j->xpos() == 0;
        case 20:
            return this->emu->j->xpos() == 2;
        case 21:
            return this->emu->j->ypos() == 2;
        case 22:
            return this->emu->j->ypos() == 0;
        case 23:
            return this->emu->j->xpos() == 1 && this->emu->j->ypos() == 1;
        case 24:
            return this->emu->m->fastinitialized();
        case 25:
            return this->emu->memexecfast;
        case 26:
            return this->emu->memadrfast;
        default:
            return false;
    }
}
Emulator emu;
ALU alu(&emu);
StackRegister s(&emu);
Memory m;
ExpansionInterface e(&emu);
Terminal t;
KeyEventQueue k;
Joystick j;
void setup() {
  Serial.begin(115200);
  lcd.begin(Config::screen_x,Config::screen_y);
  lcd.clear();
  for (int i = 0; i < 4; i++) {
      pinMode(Config::KOpins[i],0x2);
  }
  for (int i = 0; i < 4; i++) {
      pinMode(Config::KOpins[i],0x2);
  }
  emu.a = &alu;
  emu.s = &s;
  emu.m = &m;
  emu.e = &e;
  emu.t = &t;
  emu.k = &k;
  emu.j = &j;
  emu.start();
}
void loop() {}
