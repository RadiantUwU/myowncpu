#define SerialInstConst(inst0,inst1,inst2) ((unsigned int)((unsigned char)inst2) | ((unsigned int)((unsigned char)inst1) << 8) | ((unsigned int)((unsigned char)inst0) << 16))
struct SerialInst {
    unsigned char inst[3];
    SerialInst(char* inst) {
        this->inst[0] = inst[0];
        this->inst[1] = inst[1];
        this->inst[2] = inst[2];
    }
    unsigned int sw() {
        return SerialInstConst(this->inst[0],this->inst[1],this->inst[2]);
    }
    /*SerialInst(char inst[3]) {
        this->inst[0] = inst[0];
        this->inst[1] = inst[1];
        this->inst[2] = inst[2];
    }*/
    SerialInst() {}
    SerialInst(char inst0,char inst1, char inst2) {
        this->inst[0] = inst0;
        this->inst[1] = inst1;
        this->inst[2] = inst2;
    }
};
