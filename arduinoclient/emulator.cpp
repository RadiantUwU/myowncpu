class Emulator;
void ExpansionHandle(SerialInfo s);
void hardreset() {
  analogWrite(Config::resetpin, -1);
  analogWrite(Config::resetpin, 255);
};//tomodify
void halt() {while (1) {}};
long millis() {return 0;}
void delay(long am) {return;}
#define MEM_SIZE_FAST 512
class Memory {
public:
    void write(unsigned int in, unsigned char byte) {
        if (!infastmode) {
            char* data = (char*)malloc(5);
            char* ind = (char*)(&in);
            memcopy(ind,data,4);
            data[4] = byte;
            SerialSend(5,SerialInsts::MemoryWrite,data);
            free(data);
        } else {
            fastmem[in % MEM_SIZE_FAST] = byte;
        }
    }
    unsigned char read(unsigned int in) {
        char* data = (char*)malloc(4);
        char* ind = (char*)(&in);
        memcopy(ind,data,4);
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
            memcopy(ind,data,4);
            SerialSend(4,SerialInsts::GetInstruction,data);
            free(data);
            SerialInfo s;
            while (1) {
                s = SerialRead();
                #ifndef NDEBUG
                printDebug((char*)s.inst,3);
                #endif
                if (!areArraysEq(s.inst,SerialInsts::GetInstruction,3)) ExpansionHandle(s);
                else break;
            }
            return s.data;
        } else {
            unsigned char* data = (unsigned char*)malloc(4);
            if ((in % MEM_SIZE_FAST) > (MEM_SIZE_FAST - 4)) {
                data[0] = fastmem[(in + 0) % MEM_SIZE_FAST];
                data[1] = fastmem[(in + 1) % MEM_SIZE_FAST];
                data[2] = fastmem[(in + 2) % MEM_SIZE_FAST];
                data[3] = fastmem[(in + 3) % MEM_SIZE_FAST];
            } else {
                memcpy(fastmem + (in % MEM_SIZE_FAST),data,4);
            }
            return data;
        }
    }
    bool infastmode = false;
    bool initfast() {
        if (fastmem == nullptr) {
            fastmem = (unsigned char*)malloc(MEM_SIZE_FAST);
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
                    SerialSend(8,SerialInsts::MemoryBulkRead,(char*)v);
                    free(v);
                    SerialInfo s = SerialRead();
                    if (!areArraysEq(s.inst,SerialInsts::MemoryBulkRead,3)) ExpansionHandle(s);
                    else {
                        memcpy(fastmem + memadrdest,s.data,memcopysize);
                    }
                    free(s.data);
                }
                break;
            case 2:
                if (true) {
                    unsigned int* v = (unsigned int*)malloc(8);
                    v[0] = memadrdest;
                    v[1] = memcopysize;
                    SerialSend(8 + memcopysize,SerialInsts::MemoryBulkWrite,(char*)v);
                    free(v);
                    for (int i = 0; i < memcopysize; i++) sendSerial((char*)&fastmem[(memadrsource + i) % MEM_SIZE_FAST]);
                }
                break;
            case 3:
                for (int i = 0; i < memcopysize) fastmem[(memadrdest + i) % MEM_SIZE_FAST] = fastmem[(memadrsource + i) % MEM_SIZE_FAST];
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
#define __aluovfw this->overflow = 
class ALU {
public:
    Emulator* emu;
    ALU(Emulator* emu) {this->emu = emu;}
    bool overflow = false;
    unsigned char OP(ALUOperations op, unsigned char A, unsigned char B) {
        switch (op) {
            case ADD:
                __aluovfw (int)(A + B) > 255;
                return positive_modulo((A + B),256);
            case SUB:
                __aluovfw (int)(A - B) < 0;
                return positive_modulo((A - B),256);
            case MUL:
                __aluovfw (int)(A * B) > 255;
                return positive_modulo((A * B),256);
            case MOD:
                __aluovfw B == 0;
                if (B == 0) {
                    return 0;
                }
                return A % B;
            case DIV:
                __aluovfw B == 0;
                if (B == 0) {
                    return 0;
                }
                return (unsigned char)(A / B);
            case SHL:
                __aluovfw A > 128;
                return (unsigned char)(A << 1);
            case SHR:
                __aluovfw (A % 2) == 1;
                return (unsigned char)(A >> 1);
            case AND:
                __aluovfw false;
                return (A & B);
            case OR:
                __aluovfw false;
                return (A | B);
            case XOR:
                __aluovfw false;
                return (A ^ B);
            case NAND:
                __aluovfw false;
                return ~(A & B);
            case NOR:
                __aluovfw false;
                return ~(A | B);
            case XNOR:
                __aluovfw false;
                return ~(A ^ B);
            case NOT:
                __aluovfw false;
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
        memcopy(ind,data,2);
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
        memcopy(ind,data,2);
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
        memcopy(ind,data,2);
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
        memcopy(ind,data,2);
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
        memcopy(ind,data,2);
        data[2] = data_;
        SerialSend(3,SerialInsts::ExpansionRead,data);
        free(data);
    }
    void reset() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcopy(ind,data,2);
        SerialSend(2,SerialInsts::ExpansionRead,data);
        free(data);
    }
    unsigned short getID() {
        char* data = (char*)malloc(2);
        char* ind = (char*)(&(this->expadr));
        memcopy(ind,data,2);
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
        memcopy(ind,data,2);
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
        memcopy(ind,data,2);
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
                #ifndef NDEBUG
                delay(200);
                char* f = (char*)malloc(13);
                f[0 ] = hexv[(x >> 4) % 16];
                f[1 ] = hexv[(x     ) % 16];
                f[2 ] = ' ';
                f[3 ] = hexv[(y >> 4) % 16];
                f[4 ] = hexv[(y     ) % 16];
                f[5 ] = ' ';
                f[6 ] = hexv[(this->memory[x + y * Config::screen_x] >> 4) % 16];
                f[7 ] = hexv[(this->memory[x + y * Config::screen_x]     ) % 16];
                f[8 ] = ' ';
                f[9 ] = hexv[((x + y * Config::screen_x) >> 12) % 16];
                f[10] = hexv[((x + y * Config::screen_x) >>  8) % 16];
                f[11] = hexv[((x + y * Config::screen_x) >>  4) % 16];
                f[12] = hexv[((x + y * Config::screen_x)      ) % 16];
                printDebug(f,13);
                free(f);
                #endif
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
#define __pci this->pc +=
#define __pc this->pc =
#define __reg this->registers
#define __b break;
#define __abo this->pc += 4; this->rega = arg1; this->regb = arg2; this->registers[arg3 % 64] = this->a->OP
#define __ab this->pc += 3; this->rega = arg1; this->registers[arg2 % 64] = this->a->OP
#define __c (this->s->condeval(this->jumpcondition))
#define __ea this->e->expadr = 
#define __e this->e->
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
        this->a = new ALU(this);
        this->m = new Memory();
        this->s = new StackRegister(this);
        this->e = new ExpansionInterface(this);
        this->t = new Terminal();
        this->k = new KeyEventQueue();
        this->j = new Joystick;
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
        #ifndef NDEBUG
        char* f = (char*)malloc(11);
        f[0 ] = hexv[(inst >> 4) % 16];
        f[1 ] = hexv[(inst     ) % 16];
        f[2 ] = ' ';
        f[3 ] = hexv[(arg1 >> 4) % 16];
        f[4 ] = hexv[(arg1     ) % 16];
        f[5 ] = ' ';
        f[6 ] = hexv[(arg2 >> 4) % 16];
        f[7 ] = hexv[(arg2     ) % 16];
        f[8 ] = ' ';
        f[9 ] = hexv[(arg3 >> 4) % 16];
        f[10] = hexv[(arg3     ) % 16];
        printDebug(f,11);
        free(f);
        #endif
        switch (inst) {
            case 0:
                return true;
            case 1:
                __pci 1;
                __b
            case 2:
                hardreset();
                return true;
            case 3:
                this->i[0].enabled = true;
                __pci 1;
                __b
            case 4:
                this->i[0].enabled = false;
                __pci 1;
                __b
            case 5:
                __pci 1;
                this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                __pc this->i[0].address[0] + this->i[0].address[1] * 256 + this->i[0].address[2] * 65536;
                this->ininterrupt++;
                __b
            case 6:
                this->i[0].condition = arg1;
                __pci 2;
                __b
            case 7:
                this->i[0].address[0] = arg1;
                __pci 2;
                __b
            case 8:
                this->i[0].address[1] = arg1;
                __pci 2;
                __b
            case 9:
                this->i[0].address[2] = arg1;
                __pci 2;
                __b
            case 10:
                this->i[0].address[0] = arg1;
                this->i[0].address[1] = arg2;
                this->i[0].address[2] = arg3;
                __pci 4;
                __b
            case 11:
                this->i[1].enabled = true;
                __pci 1;
                __b
            case 12:
                this->i[1].enabled = false;
                __pci 1;
                __b
            case 13:
                __pci 1;
                this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                __pc this->i[1].address[0] + this->i[1].address[1] * 256 + this->i[1].address[2] * 65536;
                this->ininterrupt++;
                __b
            case 14:
                this->i[1].condition = arg1;
                __pci 2;
                __b
            case 15:
                this->i[1].address[0] = arg1;
                __pci 2;
                __b
            case 16:
                this->i[1].address[1] = arg1;
                __pci 2;
                __b
            case 17:
                this->i[1].address[2] = arg1;
                __pci 2;
                __b
            case 18:
                this->i[1].address[0] = arg1;
                this->i[1].address[1] = arg2;
                this->i[1].address[2] = arg3;
                __pci 4;
                __b
            case 19:
                this->memaddr[0] = arg1;
                __pci 2;
                __b
            case 20:
                this->memaddr[1] = arg1;
                __pci 2;
                __b
            case 21:
                this->memaddr[2] = arg1;
                __pci 2;
                __b
            case 22:
                this->memaddr[0] = arg1;
                this->memaddr[1] = arg2;
                this->memaddr[2] = arg3;
                __pci 4;
                __b
            case 23:
                this->memaddr[0]++;
                if (this->memaddr[0] == 0) {
                    this->memovfw = true;
                    this->memaddr[1]++;
                    if (this->memaddr[1] == 0) this->memaddr[2]++;
                } else this->memovfw = false;
                __pci 1;
                __b
            case 24:
                this->memaddr[1]++;
                if (this->memaddr[1] == 0) {
                    this->memovfw = true;
                    this->memaddr[2]++;
                } else this->memovfw = false;
                __pci 1;
                __b
            case 25:
                this->memaddr[2]++;
                this->memovfw = this->memaddr[2] == 0;
                __pci 1;
                __b
            case 26:
                this->memaddr[0]--;
                if (this->memaddr[0] == 255) {
                    this->memovfw = true;
                    this->memaddr[1]--;
                    if (this->memaddr[1] == 255) this->memaddr[2]--;
                } else this->memovfw = false;
                __pci 1;
                __b
            case 27:
                this->memaddr[1]--;
                if (this->memaddr[1] == 255) {
                    this->memovfw = true;
                    this->memaddr[2]--;
                } else this->memovfw = false;
                __pci 1;
                __b
            case 28:
                this->memaddr[2]--;
                this->memovfw = this->memaddr[2] == 255;
                __pci 1;
                __b
            case 29:
                if (this->m->fastinitialized()) this->m->infastmode = memadrfast;
                else this->m->infastmode = false;
                __reg[arg1 % 64] = this->m->read(this->memaddr[0] + this->memaddr[1] * 256 + this->memaddr[2] * 65536);
                if (this->memautoinc) {
                    this->memaddr[0]++;
                    if (this->memaddr[0] == 0) {
                        this->memovfw = true;
                        this->memaddr[1]++;
                        if (this->memaddr[1] == 0) this->memaddr[2]++;
                    } else this->memovfw = false;
                }
                __pci 2;
                __b
            case 30:
                if (this->m->fastinitialized()) this->m->infastmode = memadrfast;
                else this->m->infastmode = false;
                this->m->write(this->memaddr[0] + this->memaddr[1] * 256 + this->memaddr[2] * 65536,__reg[arg1 % 64]);
                if (this->memautoinc) {
                    this->memaddr[0]++;
                    if (this->memaddr[0] == 0) {
                        this->memovfw = true;
                        this->memaddr[1]++;
                        if (this->memaddr[1] == 0) this->memaddr[2]++;
                    } else this->memovfw = false;
                }
                __pci 2;
                __b
            case 31:
                __reg[arg1 % 64] = arg2;
                __pci 3;
                __b
            case 32:
                __reg[arg1 % 64]++;
                __pci 2;
                this->memovfw = __reg[arg1 % 64] == 0;
                __b
            case 33:
                __reg[arg1 % 64]--;
                __pci 2;
                this->memovfw = __reg[arg1 % 64] == 255;
                __b
            case 34:
                this->memautoinc = true;
                __pci 1;
                __b
            case 35:
                this->memautoinc = false;
                __pci 1;
                __b
            case 36:
                if (this->m->fastinitialized()) this->m->infastmode = memadrfast;
                else this->m->infastmode = false;
                if (1) {unsigned short s = this->m->getSize();
                __reg[arg1 % 64] = s % 256;
                __reg[arg2 % 64] = s >> 8;}
                __pci 3;
                __b
            case 37:
                __reg[arg1 % 64] = this->memaddr[0];
                __pci 2;
                __b
            case 38:
                __reg[arg1 % 64] = this->memaddr[1];
                __pci 2;
                __b
            case 39:
                __reg[arg1 % 64] = this->memaddr[2];
                __pci 2;
                __b
            case 40:
                __reg[arg2 % 64] = __reg[arg1 % 64];
                __pci 3;
                __b
            case 41:
                __reg[arg2 % 64] = __reg[__reg[arg1 % 64] % 64];
                __pci 3;
                __b
            case 42:
                __reg[arg1 % 64]++;
                __pci 2;
                this->a->overflow = __reg[arg1 % 64] == 0;
                __b
            case 43:
                __reg[arg1 % 64]--;
                __pci 2;
                this->a->overflow = __reg[arg1 % 64] == 255;
                __b
            case 44:
                __abo(ADD,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 45:
                __abo(SUB,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 46:
                __abo(MUL,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 47:
                __abo(MOD,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 48:
                __abo(DIV,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 49:
                __ab(SHL,__reg[arg1 % 64],0);
                __b
            case 50:
                __ab(SHR,__reg[arg1 % 64],0);
                __b
            case 51:
                __abo(AND,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 52:
                __abo(OR,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 53:
                __abo(XOR,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 54:
                __abo(NAND,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 55:
                __abo(NOR,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 56:
                __abo(XNOR,__reg[arg1 % 64],__reg[arg2 % 64]);
                __b
            case 57:
                __pci 3;
                this->rega = arg1;
                this->regb = arg2;
                __b
            case 58:
                __ab(SHR,__reg[arg1 % 64],0);
                __b
            case 59:
                this->jumpcondition = arg1;
                __pci 2;
                __b
            case 60: 
                __pc arg1 + arg2 * 256 + arg3 * 65536;
                __b
            case 61:
                if (__c) __pc arg1 + arg2 * 256 + arg3 * 65536;
                else __pci 4;
                __b
            case 62:
                __pci 4;
                this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                __pc arg1 + arg2 * 256 + arg3 * 65536;
                if (this->ininterrupt != 0) this->ininterrupt++;
                __b
            case 63:
                __pci 4;
                if (__c) {
                    this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                    __pc arg1 + arg2 * 256 + arg3 * 65536;
                    if (this->ininterrupt != 0) this->ininterrupt++;
                }
                __b
            case 64:
                if (!__c) __pc arg1 + arg2 * 256 + arg3 * 65536;
                else __pci 4;
                __b
            case 65:
                __pci 4;
                if (!__c) {
                    this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
                    __pc arg1 + arg2 * 256 + arg3 * 65536;
                    if (this->ininterrupt != 0) this->ininterrupt++;
                }
                __b
            case 66:
                this->ret[0] = 255;
                this->ret[1] = 255;
                this->ret[2] = 255;
                if (1) {unsigned char* a = this->s->getaddy();
                __pc a[0] + a[1] * 256 + a[2] * 65536;
                free(a);}
                if (this->ininterrupt != 0) this->ininterrupt--;
                __b
            case 67:
                this->ret[0] = arg1;
                __pci 2;
                __b
            case 68:
                this->ret[1] = arg1;
                __pci 2;
                __b
            case 69:
                this->ret[2] = arg1;
                __pci 2;
                __b
            case 70:
                __reg[arg1 % 64] = this->ret[0];
                __pci 2;
                __b
            case 71:
                __reg[arg1 % 64] = this->ret[1];
                __pci 2;
                __b
            case 72:
                __reg[arg1 % 64] = this->ret[2];
                __pci 2;
                __b
            case 73:
                this->ret[0] = arg1;
                this->ret[1] = arg2;
                this->ret[2] = arg3;
                if (1) {unsigned char* a = this->s->getaddy();
                __pc a[0] + a[1] * 256 + a[2] * 65536;
                free(a);}
                if (this->ininterrupt != 0) this->ininterrupt--;
                __b
            case 74:
                this->s->push(__reg[arg1 % 64]);
                __pci 2;
                __b
            case 75:
                __reg[arg1 % 64] = this->s->pop();
                __pci 2;
                __b
            case 76:
                this->s->stackpointer = __reg[arg1 % 64];
                __pci 2;
                __b
            case 77:
                __reg[arg1 % 64] = this->s->stackpointer;
                __pci 2;
                __b
            case 78:
                if (true) {
                    KeyEvent k = this->k->pull();
                    __reg[arg1 % 64] = k.key;
                    __reg[arg2 % 64] = (unsigned int)k.isdown * 128;
                }
                __pci 3;
                __b
            case 79:
                this->k->enabled = true;
                __pci 1;
                __b
            case 80:
                this->k->enabled = false;
                __pci 1;
                __b
            case 81:
                this->k->k = __reg[arg1 % 64];
                __pci 3;
                __b;
            case 82:
                this->k->p = 0;
                __pci 1;
                __b
            case 83:
                for (int i = 0; i < this->t->size; i++) {
                    this->t->memory[i] = 0;
                }
                __pci 1;
                __b
            case 84:
                this->t->refresh();
                __pci 1;
                __b
            case 85:
                this->t->put(__reg[arg1 % 64]);
                __pci 2;
                __b
            case 86:
                for (int i = 0; i < this->t->size; i++) {
                    this->t->memory[i] = __reg[arg1 % 64];
                }
                __pci 2;
                __b
            case 87:
                for (int i = 0; i < this->t->size; i++) {
                    this->t->memory[i] = 0;
                }
                this->t->x = 0;
                this->t->y = 0;
                this->t->autoincrement = false;
                this->t->refresh();
                __pci 1;
                __b
            case 88:
                this->t->x = __reg[arg1 % 64];
                __pci 2;
                __b
            case 89:
                this->t->y = __reg[arg1 % 64];
                __pci 2;
                __b
            case 90:
                this->t->x = __reg[arg1 % 64];
                this->t->y = __reg[arg2 % 64];
                __pci 3;
                __b
            case 91:
                this->t->write(__reg[arg1 % 64] + __reg[arg2 % 64] * 256,__reg[arg3 % 64]);
                __pci 4;
                __b
            case 92:
                __pci 3;
                __b
            case 93:
                __pci 2;
                __b
            case 94:
                __pci 2;
                __b
            case 95:
                this->tt[arg1 % 64].inc += __reg[arg2 % 64] + 1;
                __pci 3;
                __b
            case 96:
                this->tt[arg1 % 64].dec += __reg[arg2 % 64] + 1;
                __pci 3;
                __b
            case 97:
                this->tt[arg1 % 64].inc = 0;
                this->tt[arg1 % 64].dec = 0;
                __pci 2;
                __b
            case 98:
                this->ttovfw = false;
                __pci 1;
                __b
            case 99:
                __reg[arg3 % 64] = this->t->read(__reg[arg1 % 64] + __reg[arg2 % 64] * 256);
                __pci 4;
                __b
            case 100:
                __reg[arg1 % 64] = 0;
                __pci 2;
                __b
            case 101:
                __reg[arg1 % 64] = 1;
                __pci 2;
                __b
            case 102:
                __ea __reg[arg1 % 64];
                __e reset();
                __pci 2;
                __b
            case 103:
                __ea __reg[arg1 % 64];
                __reg[arg2 % 64] = __e ask();
                __pci 3;
                __b
            case 104:
                __ea __reg[arg1 % 64];
                __reg[arg2 % 64] = __e read();
                __pci 3;
                __b
            case 105:
                __ea __reg[arg1 % 64];
                __e send(__reg[arg2 % 64]);
                __pci 3;
                __b
            case 106:
                __ea __reg[arg1 % 64];
                __reg[arg2 % 64] = __e wait();
                __pci 3;
                __b
            case 107:
                __e resetAll();
                __pci 1;
                __b
            case 108:
                __ea __reg[arg1 % 64];
                if (true) {
                    unsigned short s = __e getID();
                    __reg[arg2 % 64] = s % 256;
                    __reg[arg3 % 64] = s >> 8;
                }
                __pci 4;
                __b
            case 109:
                __ea __reg[arg1 % 64];
                __reg[arg2 % 64] = __e ping();
                __pci 3;
                __b
            case 110:
                __ea __reg[arg1 % 64];
                __pci 2;
                __b
            case 111:
                this->freqlocked = true;
                this->cfr = arg1 + 1 + arg2 * 256;
                __pci 3;
                __b
            case 112:
                this->freqlocked = false;
                __pci 1;
                __b
            case 113:
                this->t->autoincrement = true;
                __pci 1;
                __b
            case 114:
                this->t->autoincrement = false;
                __pci 1;
                __b
            case 115:
                __reg[arg1 % 64] = this->j->xpos();
                __pci 2;
                __b
            case 116:
                __reg[arg1 % 64] = this->j->ypos();
                __pci 2;
                __b
            case 117:
                tone(Config::speakerpin,__reg[arg1 % 64] + __reg[arg2 % 64] * 256);
                __pci 3;
                __b
            case 118:
                noTone(Config::speakerpin);
                __pci 1;
                __b
            case 119:
                __pci 4;
                this->s->pushaddy(__reg[arg1 % 64], __reg[arg2 % 64], __reg[arg3 % 64]);
                __b
            case 120:
                __pci 4;
                if (true) {
                    unsigned char *f = this->s->getaddy();
                    __reg[arg1 % 64] = f[0];
                    __reg[arg2 % 64] = f[1];
                    __reg[arg3 % 64] = f[2];
                    free(f);
                }
                __b
            case 121:
                __pci 1;
                if (this->m->fastinitialized()) {
                    this->willexecfast = !this->memexecfast;                    
                }
                __b
            case 122:
                __pci 1;
                this->willexecfast = this->memexecfast;
                __b
            case 123:
                __pci 1;
                if(this->m->fastinitialized()) {
                    this->memadrfast = !this->memadrfast;
                }
                __b
            case 124:
                __pci 1;
                this->m->initfast();
                __b
            case 125:
                __pci 1;
                this->m->uninitfast();
                this->memexecfast = 0;
                this->memadrfast = 0;
                this->willexecfast = 0;
                __b
            case 126:
                __pci 1;
                this->m->memadrsource = this->memaddr[0] + this->memaddr[1] * 256 + this->memaddr[2] * 65536;
                __b
            case 127:
                __pci 1;
                this->m->memadrdest = this->memaddr[0] + this->memaddr[1] * 256 + this->memaddr[2] * 65536;
                __b
            case 128:
                __pci 3;
                this->m->copyfromfast = arg1 % 2;
                this->m->copytofast = arg2 % 2;
                __b
            case 129:
                __pci 4;
                this->m->memcopysize = arg1 + 1 + arg2 * 256 + arg3 * 65536;
                __b
            case 130:
                __pci 1;
                this->m->memcopy();
                __b
            default:
                return true;
        }
        return false;
    };
    void interruptcheck() {
        if (this->ininterrupt != 0) return;
        if (this->i[0].evaluate()) {
            this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
            __pc this->i[0].address[0] + this->i[0].address[1] * 256 + this->i[0].address[2] * 65536;
            this->ininterrupt++;
            return;
        }
        if (this->i[1].evaluate()) {
            this->s->pushaddy(this->pc % 256, (this->pc >> 8) % 256, (this->pc >> 16) % 256);
            __pc this->i[1].address[0] + this->i[1].address[1] * 256 + this->i[1].address[2] * 65536;
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
            memcopy(data,inst,4);
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
void setup() {
  Serial.begin(115200);
  lcd.begin(Config::screen_x,Config::screen_y);
  lcd.clear();
  for (int i = 0; i < 4; i++) {
      pinMode(Config::KOpins[i],INPUT_PULLUP);
  }
  for (int i = 0; i < 4; i++) {
      pinMode(Config::KOpins[i],INPUT_PULLUP);
  }
  Emulator emu = Emulator();
  emu.start();
}
void loop() {}