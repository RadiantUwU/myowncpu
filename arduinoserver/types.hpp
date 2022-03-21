struct SerialInst {
    union {
        unsigned char inst[3];
        unsigned int sw;
    };
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