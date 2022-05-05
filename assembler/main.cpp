#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

#include "../head/assembler.hpp"

#define NDEBUG

Assembler a;
void init_asm() {
    a.consts = {
        {"CSH",0},{"NOP",1},{"RST",2},
        {"EI1",3},{"DI1",4},{"IN1",5},{"IC1",6},{"IL1",7},{"IM1",8},{"IH1",9},{"IA1",10},
        {"EI2",11},{"DI2",12},{"IN2",13},{"IC2",14},{"IL2",15},{"IM2",16},{"IH2",17},{"IA2",18},
        {"SAL",19},{"SAM",20},{"SAH",21},{"SEA",22},{"INL",23},{"INM",24},{"INH",25},{"DEL",26},{"DEM",27},{"DEH",28},
        {"LDR",29},{"STR",30},{"SER",31},{"IMR",32},{"DMR",33},{"EAI",34},{"DAI",35},
        {"MSK",36},{"RAL",37},{"RAM",38},{"RAH",39},{"MOV",40},{"MVS",41},{"IAR",42},{"DAR",43},
        {"ADD",44},{"SUB",45},{"MUL",46},{"MOD",47},{"DIV",48},{"SHL",49},{"SHR",50},{"AND",51},{"OR", 52},{"XOR",53},{"NAD",54},{"NOR",55},{"NXR",56},{"SAB",57},{"NOT",58},{"SJC",59},
        {"JMP",60},{"JMI",61},{"JSR",62},{"JSI",63},{"JMN",64},{"JSN",65},{"REV",66},{"RRA",67},{"RRB",68},{"RRC",69},{"GRA",70},{"GRB",71},{"GRC",72},{"RET",73},{"PSH",74},{"POP",75},{"SRP",76},{"RSP",77},
        {"KER",78},{"ENK",79},{"DIK",80},{"KKS",81},{"CKB",82},
        {"TME",83},{"TMU",84},{"TMS",85},{"TMF",86},{"TMR",87},{"TMX",88},{"TMY",89},{"TMP",90},{"TMW",91},{"TMC",92},{"TMM",93},{"TAS",94},{"TRI",95},{"TRD",96},{"TRR",97},{"TRF",98},{"TRM",99},{"TKT",100},{"TST",101},
        {"EXQ",102},{"EXG",103},{"EXR",104},{"EXS",105},{"EXW",106},{"EXF",107},{"EXI",108},{"EXP",109},{"EXA",110},
        {"LCS",111},{"UCS",112},
        {"TEA",113},{"TDA",114},
        {"JXR",115},{"JYR",116},
        {"BEP",117},{"STB",118},
        {"PSA",119},{"POA",120},
        {"SWP",121},{"CSP",122},{"SWA",123},{"IFM",124},{"UFM",125},
        {"MEMCPYA1",126},{"MEMCPYA2",127},{"MEMSET",128},{"MEMCSZ",129},{"MEMCPY",130},
        {"$temp0",63},{"$temp1",62},{"$temp2",61},{"$temp3",60}

    };
    a.addrlen = 3;
}
int main(int argc, char** argv) {
    init_asm();
    init_color();
    bool makinginf = false;
    bool makingouf = false;
    char* infile = nullptr;
    char* outfile = nullptr;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'i':
                    makinginf = true;
                    break;
                case 'o':
                    makingouf = true;
                    break;
                case 'v':
                    a.verbose = true;
            }
            continue;
        }
        if (makinginf) {
            infile = argv[i];
            makinginf = false;
        } else if (makingouf) {
            outfile = argv[i];
            makingouf = false;
        }
    }
    #ifndef NDEBUG
        infile = (char*)"input.txt";
        outfile = (char*)"rom.txt";
        a.verbose = true;
    #endif
    if (infile == nullptr) {
        std::cout << COLOR_WHITE COLOR_BACKGROUND_RED "0 FATAL " "No input file given." COLOR_RESET << std::endl;
        return 1;
    } else if (outfile == nullptr) {
        std::cout << COLOR_WHITE COLOR_BACKGROUND_RED "0 FATAL " "No output file given." COLOR_RESET << std::endl;
        return 1;
    }
    std::ifstream inpfile(infile);
    if (!inpfile.is_open()) {
        std::cout << COLOR_WHITE COLOR_BACKGROUND_RED "0 FATAL " "Could not open input file." COLOR_RESET << std::endl;
        return 1;
    }
    std::ofstream outpfile(outfile);
    if (!outpfile.is_open()) {
        std::cout << COLOR_WHITE COLOR_BACKGROUND_RED "0 FATAL " "Could not open output file." COLOR_RESET << std::endl;
        inpfile.close();
        return 1;
    }
    char v;
    std::string af;
    while (true) {
        inpfile.get(v);
        if (inpfile.eof()) break;
        af += v;
    }
    inpfile.close();
    std::vector<unsigned char> out = a.build(af);
    std::stringstream ss;
    ss << std::hex;
    for (unsigned char c : out) {
        ss << (unsigned int)c << " ";
    }
    std::string out_str = ss.str();
    out_str.pop_back();
    outpfile << out_str;
    outpfile.close();
    return 0;
}