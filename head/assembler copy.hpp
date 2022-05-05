#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <exception>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "stringUtilities.hpp"
#include "vectorUtilites.hpp"
#include "unorderedmapUtilities.hpp"
#include "colors.hpp"

#pragma once

#define HALF_RAND (((rand() & 0x7FFF) << 1) + (rand() & 0x1))
#define FULL_RAND ((HALF_RAND << 16) + HALF_RAND)

typedef std::runtime_error RuntimeError;
namespace fs = std::filesystem;
namespace __assembler_namespace {
    using namespace std;
    using namespace std::chrono;
    long long getTime() {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
    struct Definition {
        string def;
        Definition(string def) : def(def) {}
        Definition() {}
    };
    template <typename valueT>
    struct CodeSegment {
        string code;
        valueT value;
        bool is_string = false;
        CodeSegment(string code) : code(code), is_string(true) {}
        CodeSegment(unsigned char value) : value(value) {}
        CodeSegment(const CodeSegment& c) {
            if (c.is_string) {
                code = c.code;
                is_string = true;
            } else {
                value = c.value;
                is_string = false;
            }
        }
        //do copy operator
        CodeSegment& operator=(const CodeSegment& c) {
            if (c.is_string) {
                code = c.code;
                is_string = true;
            } else {
                value = c.value;
                is_string = false;
            }
            return *this;
        }
        //thank you
        //no problem
        //thank you

        ~CodeSegment() {
            if (is_string) {
                code.clear();
            }
        }
    };
    template <typename valueT>
    struct AsmCode_Obj {
        vector<CodeSegment<valueT>> code;
        unsigned int pos = 0;
        void reset() {
            this->code.clear();
            this->pos = 0;
        }
        ~AsmCode_Obj() {
            this->code.clear();
        }
    };
    struct temp_inst_t {
        unsigned long long i;
        string s;
        temp_inst_t() {}
        ~temp_inst_t() {
            s.clear();
        }
    };
    template <uint8_t bits>
    class OldAssembler {
    public:
        unsigned char addrlen = 2;
        //std::unordered_map<std::string,unsigned char> consts;
        void start_build(string file) {
            clock = getTime();
            print_info("Build started.");
            print_debug("Callstack main");
            callstack.push_back("main");
            assemble(file);
        }
        void assemble(string file);
        vector <unsigned char> finalize() {
            if (inst != 0) {
                print_fatal_error("unfinished preprocessor instruction");
                print_debug("Printing callstack");
                for (string s : callstack) {
                    print_debug(s);
                }
                throw RuntimeError("unfinished preprocessor instruction");
            };
            vector<unsigned char> ret = exp;
            exp.clear();
            labels.clear();
            pos = 0;
            print_debug("Exited callstack main");
            print_info("Build finished.");
            return ret;
        }
        bool verbose = false;
    protected:
        /*vector <unsigned char> numInterpretAdr(string buffer) {
            vector <unsigned char> ret;
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) {
                        bool gud = true;
                        unsigned int i;
                        switch (buffer[1]) {
                            case 'x':
                                i = stoll(buffer.substr(2), nullptr, 16);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFF);
                                    i >>= 8;
                                }
                                break;
                            case 'o':
                                i = stoll(buffer.substr(2), nullptr, 8);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFF);
                                    i >>= 8;
                                }
                                break;
                            case 'b':
                                i = stoll(buffer.substr(2), nullptr, 2);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFF);
                                    i >>= 8;
                                }
                                break;
                            case 'c':
                                ret.push_back(buffer[2] & 0xFF);
                            default:
                                gud = false;
                                break;
                        }
                        if (gud) break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    unsigned int i = stoll(buffer, nullptr, 10);
                    for (int j = 0; j < addrlen; j++) {
                        ret.push_back(i & 0xFF);
                        i >>= 8;
                    }
                    break;
            }
            return ret;
            
        }
        vector <unsigned char> numInterpret(string buffer) {
            vector <unsigned char> ret;
            unsigned int i;
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) {
                        bool gud = true;
                        switch (buffer[1]) {
                            case 'x':
                                i = stoll(buffer.substr(2), nullptr, 16);
                                ret.push_back(i & 0xFF);
                                break;
                            case 'o':
                                i = stoll(buffer.substr(2), nullptr, 8);
                                ret.push_back(i & 0xFF);
                                break;
                            case 'b':
                                i = stoll(buffer.substr(2), nullptr, 2);
                                ret.push_back(i & 0xFF);
                                break;
                            case 'c':
                                ret.push_back(buffer[2] & 0xFF);
                                break;
                            case 's':
                                for (int i = 3; i < buffer.length() - 1; i++) {
                                    ret.push_back(buffer[i] & 0xFF);
                                }
                                break;
                            default:
                                gud = false;
                                break;
                        }
                        if (gud) break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    i = stoll(buffer, nullptr, 10);
                    ret.push_back(i & 0xFF);
                    break;
            }
            return ret;
            
        }*/
        unsigned long long numInterpretInt(string buffer) {
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) switch (buffer[1]) {
                        case 'x':
                            return stoll(buffer.substr(2), nullptr, 16);
                        case 'o':
                            return stoll(buffer.substr(2), nullptr, 8);
                        case 'b':
                            return stoll(buffer.substr(2), nullptr, 2);
                        case 'c':
                            return buffer[2];
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    return stoll(buffer, nullptr, 10);
            }
            throw RuntimeError("Not a number.");
        }
        unordered_map<string,unsigned long long> labels;
        unordered_map<string,Definition> definitions;
        //vector <unsigned char> exp;
        unsigned long long pos = 0;
        bool inString = false;
        string str_buf_curr;
        unsigned long long clock;
        vector <string> callstack;
        void print_info(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_DARK_CYAN << round(getTime() - clock) / 1000 << " INFO " << info << COLOR_RESET << endl;
        }
        void print_error(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_RED << round(getTime() - clock) / 1000 << " ERROR " << info << COLOR_RESET << endl;
        }
        void print_warning(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_YELLOW << round(getTime() - clock) / 1000 << " WARN " << info << COLOR_RESET << endl;
        }
        void print_debug(string info) {
            if (!verbose) return;
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_GREEN << round(getTime() - clock) / 1000 << " DEBUG " << info << COLOR_RESET << endl;
        }
        void print_fatal_error(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_WHITE COLOR_BACKGROUND_DARK_RED << round(getTime() - clock) / 1000 << " FATAL " << info << COLOR_RESET << endl;
        }
    private:
        bool inComment = false;
        unsigned char inst = 0;
        unsigned int posix = 0;
        unsigned int posF = 0;
        string buffer_def;
        string buffer__def;
    };
    class OldAssembler<8> {
    public:
        unsigned char addrlen = 2;
        std::unordered_map<std::string,unsigned char> consts;
        void start_build(string file) {
            clock = getTime();
            print_info("Build started.");
            print_debug("Callstack main");
            callstack.push_back("main");
            assemble(file);
        }
        void assemble(string file) {
            string buffer;
            file += " ";
            for (unsigned char a : file) {
                switch(a) {
                    case '\r':
                    case '\n':
                    case ' ':
                        if (inComment) {
                            if (isIn(buffer, "*/")) {
                                inComment = false;
                                buffer = split(buffer, "*/")[1];
                            } else buffer.clear();
                        }
                        if (buffer.length() == 0) break;
                        switch (inst) {
                            case 0:
                                break;
                            case 'o':
                                pos = numInterpretInt(buffer);
                                buffer.clear();
                                inst = 0;
                                break;
                            case 'e':
                                if (true) {
                                    unsigned int idc = numInterpretInt(buffer);
                                    vector<unsigned char> f(idc,0);
                                    exp.insert(exp.end(), f.begin(), f.end());
                                    pos += idc;
                                    buffer.clear();
                                    inst = 0;
                                    break;
                                }
                            case 'f':
                                if (posix == 0) {
                                    posF = numInterpretInt(buffer);
                                    buffer.clear();
                                    posix++;
                                    break;
                                } else {
                                    vector<unsigned char> f(posF,numInterpretInt(buffer));
                                    pos += posF;
                                    exp.insert(exp.end(), f.begin(), f.end());
                                    buffer.clear();
                                    inst = 0;
                                    break;
                                }
                            case 'd':
                                if (posix == 0) {
                                    buffer_def = buffer;
                                    buffer.clear();
                                    posix++;
                                    break;
                                } else {
                                    if (buffer == ".end") {
                                        posix--;
                                        if (posix == 0) {
                                            buffer__def.pop_back();
                                            definitions[buffer_def] = Definition{buffer__def};
                                            buffer_def.clear();
                                            buffer__def.clear();
                                            buffer.clear();
                                            inst = 0;
                                            break;
                                        }
                                        buffer__def += buffer + " ";
                                        break;
                                    } else if (buffer == ".def") {
                                        posix++;
                                        buffer__def += buffer + " ";
                                        buffer.clear();
                                        break;
                                    } else if (buffer == buffer_def) {
                                        print_fatal_error("Definition name is the same as the instruction name.");
                                        print_info("Macro name: " + buffer);
                                        print_debug("Printing callstack.");
                                        for (string s : callstack) {
                                            print_debug(s);
                                        }
                                        throw RuntimeError("Definition name is the same as the instruction name.");
                                    } else {
                                        buffer__def += buffer + " ";
                                        buffer.clear();
                                        break;
                                    }
                                }
                            case 'u':
                                definitions.erase(buffer);
                                buffer.clear();
                                inst = 0;
                                break;
                        }
                        if (buffer.length() == 0) break;
                        switch(buffer[0]) {
                                case '0':
                                case '1':
                                case '2':
                                case '3':
                                case '4':
                                case '5':
                                case '6':
                                case '7':
                                case '8':
                                case '9':
                                    if (true) {
                                        vector<unsigned char> v = numInterpret(buffer);
                                        exp.insert(exp.end(),v.begin(),v.end());
                                        pos += v.size();
                                    }
                                case ':':
                                    labels[buffer.substr(1)] = pos;
                                    break;
                                case '.':
                                    buffer = buffer.substr(1);
                                    if (buffer == "org") inst = 'o';
                                    else if (buffer == "emp") inst = 'e';
                                    else if (buffer == "fill") inst = 'f';
                                    else if (buffer == "def") inst = 'd';
                                    else if (buffer == "undef") inst = 'u';
                                    break;
                                case '/':
                                    if (buffer == "/*") {
                                        inComment = true;
                                        break;
                                    }
                                default:
                                    if (consts.find(buffer) != consts.end()) {
                                        exp.push_back(consts[buffer]);
                                        pos++;
                                    } else if (labels.find(buffer) != labels.end()) {
                                        unsigned char* l = (unsigned char*)&(labels[buffer]);
                                        for (int i = 0; i < addrlen; i++) exp.push_back(l[i]);
                                        pos += addrlen;
                                    } else if (definitions.find(buffer) != definitions.end()) {
                                        callstack.push_back(buffer);
                                        print_debug("Callstack " + buffer);
                                        pos++;
                                        assemble(definitions[buffer].def);
                                        print_debug("Exited callstack " + buffer);
                                        callstack.pop_back();
                                    } else {

                                    }
                            }
                        buffer.clear();
                        break;
                    default:
                        buffer += a;
                        break;
                }
            }
        }
        vector <unsigned char> finalize() {
            if (inst != 0) {
                print_fatal_error("unfinished preprocessor instruction");
                print_debug("Printing callstack");
                for (string s : callstack) {
                    print_debug(s);
                }
                throw RuntimeError("unfinished preprocessor instruction");
            };
            vector<unsigned char> ret = exp;
            exp.clear();
            labels.clear();
            pos = 0;
            print_debug("Exited callstack main");
            print_info("Build finished.");
            return ret;
        }
        void test() {
            clock = getTime();
            print_debug("test");
            print_info("test");
            print_warning("test");
            print_error("test");
            print_fatal_error("test");
        }
        bool verbose = false;
    protected:
        vector <unsigned char> numInterpretAdr(string buffer) {
            vector <unsigned char> ret;
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) {
                        bool gud = true;
                        unsigned int i;
                        switch (buffer[1]) {
                            case 'x':
                                i = stoll(buffer.substr(2), nullptr, 16);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFF);
                                    i >>= 8;
                                }
                                break;
                            case 'o':
                                i = stoll(buffer.substr(2), nullptr, 8);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFF);
                                    i >>= 8;
                                }
                                break;
                            case 'b':
                                i = stoll(buffer.substr(2), nullptr, 2);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFF);
                                    i >>= 8;
                                }
                                break;
                            case 'c':
                                ret.push_back(buffer[2] & 0xFF);
                            default:
                                gud = false;
                                break;
                        }
                        if (gud) break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    unsigned int i = stoll(buffer, nullptr, 10);
                    for (int j = 0; j < addrlen; j++) {
                        ret.push_back(i & 0xFF);
                        i >>= 8;
                    }
                    break;
            }
            return ret;
            
        }
        vector <unsigned char> numInterpret(string buffer) {
            vector <unsigned char> ret;
            unsigned int i;
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) {
                        bool gud = true;
                        switch (buffer[1]) {
                            case 'x':
                                i = stoll(buffer.substr(2), nullptr, 16);
                                ret.push_back(i & 0xFF);
                                break;
                            case 'o':
                                i = stoll(buffer.substr(2), nullptr, 8);
                                ret.push_back(i & 0xFF);
                                break;
                            case 'b':
                                i = stoll(buffer.substr(2), nullptr, 2);
                                ret.push_back(i & 0xFF);
                                break;
                            case 'c':
                                ret.push_back(buffer[2] & 0xFF);
                                break;
                            case 's':
                                for (int i = 3; i < buffer.length() - 1; i++) {
                                    ret.push_back(buffer[i] & 0xFF);
                                }
                                break;
                            default:
                                gud = false;
                                break;
                        }
                        if (gud) break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    i = stoll(buffer, nullptr, 10);
                    ret.push_back(i & 0xFF);
                    break;
            }
            return ret;
            
        }
        unsigned long long numInterpretInt(string buffer) {
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) switch (buffer[1]) {
                        case 'x':
                            return stoll(buffer.substr(2), nullptr, 16);
                        case 'o':
                            return stoll(buffer.substr(2), nullptr, 8);
                        case 'b':
                            return stoll(buffer.substr(2), nullptr, 2);
                        case 'c':
                            return buffer[2];
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    return stoll(buffer, nullptr, 10);
            }
            throw RuntimeError("Not a number.");
        }
        unordered_map<string,unsigned int> labels;
        unordered_map<string,Definition> definitions;
        vector <unsigned char> exp;
        unsigned int pos = 0;
        bool inString = false;
        string str_buf_curr;
        unsigned long long clock;
        vector <string> callstack;
        void print_info(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_DARK_CYAN << round(getTime() - clock) / 1000 << " INFO " << info << COLOR_RESET << endl;
        }
        void print_error(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_RED << round(getTime() - clock) / 1000 << " ERROR " << info << COLOR_RESET << endl;
        }
        void print_warning(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_YELLOW << round(getTime() - clock) / 1000 << " WARN " << info << COLOR_RESET << endl;
        }
        void print_debug(string info) {
            if (!verbose) return;
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_GREEN << round(getTime() - clock) / 1000 << " DEBUG " << info << COLOR_RESET << endl;
        }
        void print_fatal_error(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_WHITE COLOR_BACKGROUND_DARK_RED << round(getTime() - clock) / 1000 << " FATAL " << info << COLOR_RESET << endl;
        }
    private:
        bool inComment = false;
        unsigned char inst = 0;
        unsigned int posix = 0;
        unsigned int posF = 0;
        string buffer_def;
        string buffer__def;
    };
    class OldAssembler<16> {
    public:
        unsigned char addrlen = 2;
        std::unordered_map<std::string,unsigned short> consts;
        void start_build(string file) {
            clock = getTime();
            print_info("Build started.");
            print_debug("Callstack main");
            callstack.push_back("main");
            assemble(file);
        }
        void assemble(string file);
        vector <unsigned short> finalize() {
            if (inst != 0) {
                print_fatal_error("unfinished preprocessor instruction");
                print_debug("Printing callstack");
                for (string s : callstack) {
                    print_debug(s);
                }
                throw RuntimeError("unfinished preprocessor instruction");
            };
            vector<unsigned short> ret = exp;
            exp.clear();
            labels.clear();
            pos = 0;
            print_debug("Exited callstack main");
            print_info("Build finished.");
            return ret;
        }
        bool verbose = false;
    protected:
        vector <unsigned short> numInterpretAdr(string buffer) {
            vector <unsigned short> ret;
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) {
                        bool gud = true;
                        unsigned int i;
                        switch (buffer[1]) {
                            case 'x':
                                i = stoll(buffer.substr(2), nullptr, 16);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFFFF);
                                    i >>= 16;
                                }
                                break;
                            case 'o':
                                i = stoll(buffer.substr(2), nullptr, 8);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFFFF);
                                    i >>= 16;
                                }
                                break;
                            case 'b':
                                i = stoll(buffer.substr(2), nullptr, 2);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFFFF);
                                    i >>= 16;
                                }
                                break;
                            case 'c':
                                ret.push_back(buffer[2] & 0xFFFF);
                            default:
                                gud = false;
                                break;
                        }
                        if (gud) break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    unsigned int i = stoll(buffer, nullptr, 10);
                    for (int j = 0; j < addrlen; j++) {
                        ret.push_back(i & 0xFFFF);
                        i >>= 16;
                    }
                    break;
            }
            return ret;
            
        }
        vector <unsigned short> numInterpret(string buffer) {
            vector <unsigned short> ret;
            unsigned int i;
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) {
                        bool gud = true;
                        switch (buffer[1]) {
                            case 'x':
                                i = stoll(buffer.substr(2), nullptr, 16);
                                ret.push_back(i & 0xFFFF);
                                break;
                            case 'o':
                                i = stoll(buffer.substr(2), nullptr, 8);
                                ret.push_back(i & 0xFFFF);
                                break;
                            case 'b':
                                i = stoll(buffer.substr(2), nullptr, 2);
                                ret.push_back(i & 0xFFFF);
                                break;
                            case 'c':
                                ret.push_back(buffer[2] & 0xFFFF);
                                break;
                            case 's':
                                for (int i = 3; i < buffer.length() - 1; i++) {
                                    ret.push_back(buffer[i] & 0xFFFF);
                                }
                                break;
                            default:
                                gud = false;
                                break;
                        }
                        if (gud) break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    i = stoll(buffer, nullptr, 10);
                    ret.push_back(i & 0xFFFF);
                    break;
            }
            return ret;
            
        }
        unsigned long long numInterpretInt(string buffer) {
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) switch (buffer[1]) {
                        case 'x':
                            return stoll(buffer.substr(2), nullptr, 16);
                        case 'o':
                            return stoll(buffer.substr(2), nullptr, 8);
                        case 'b':
                            return stoll(buffer.substr(2), nullptr, 2);
                        case 'c':
                            return buffer[2];
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    return stoll(buffer, nullptr, 10);
            }
            throw RuntimeError("Not a number.");
        }
        unordered_map<string,unsigned int> labels;
        unordered_map<string,Definition> definitions;
        vector <unsigned short> exp;
        unsigned int pos = 0;
        bool inString = false;
        string str_buf_curr;
        unsigned long long clock;
        vector <string> callstack;
        void print_info(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_DARK_CYAN << round(getTime() - clock) / 1000 << " INFO " << info << COLOR_RESET << endl;
        }
        void print_error(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_RED << round(getTime() - clock) / 1000 << " ERROR " << info << COLOR_RESET << endl;
        }
        void print_warning(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_YELLOW << round(getTime() - clock) / 1000 << " WARN " << info << COLOR_RESET << endl;
        }
        void print_debug(string info) {
            if (!verbose) return;
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_GREEN << round(getTime() - clock) / 1000 << " DEBUG " << info << COLOR_RESET << endl;
        }
        void print_fatal_error(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_WHITE COLOR_BACKGROUND_DARK_RED << round(getTime() - clock) / 1000 << " FATAL " << info << COLOR_RESET << endl;
        }
    private:
        bool inComment = false;
        unsigned char inst = 0;
        unsigned int posix = 0;
        unsigned int posF = 0;
        string buffer_def;
        string buffer__def;
    };
    class OldAssembler<32> {
    public:
        unsigned char addrlen = 2;
        std::unordered_map<std::string,unsigned long> consts;
        void start_build(string file) {
            clock = getTime();
            print_info("Build started.");
            print_debug("Callstack main");
            callstack.push_back("main");
            assemble(file);
        }
        void assemble(string file);
        vector <unsigned long> finalize() {
            if (inst != 0) {
                print_fatal_error("unfinished preprocessor instruction");
                print_debug("Printing callstack");
                for (string s : callstack) {
                    print_debug(s);
                }
                throw RuntimeError("unfinished preprocessor instruction");
            };
            vector<unsigned long> ret = exp;
            exp.clear();
            labels.clear();
            pos = 0;
            print_debug("Exited callstack main");
            print_info("Build finished.");
            return ret;
        }
        bool verbose = false;
    protected:
        vector <unsigned long> numInterpretAdr(string buffer) {
            vector <unsigned long> ret;
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) {
                        bool gud = true;
                        unsigned int i;
                        switch (buffer[1]) {
                            case 'x':
                                i = stoll(buffer.substr(2), nullptr, 16);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFFFF);
                                    i >>= 16;
                                }
                                break;
                            case 'o':
                                i = stoll(buffer.substr(2), nullptr, 8);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFFFF);
                                    i >>= 16;
                                }
                                break;
                            case 'b':
                                i = stoll(buffer.substr(2), nullptr, 2);
                                for (int j = 0; j < addrlen; j++) {
                                    ret.push_back(i & 0xFFFF);
                                    i >>= 16;
                                }
                                break;
                            case 'c':
                                ret.push_back(buffer[2] & 0xFFFF);
                            default:
                                gud = false;
                                break;
                        }
                        if (gud) break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    unsigned int i = stoll(buffer, nullptr, 10);
                    for (int j = 0; j < addrlen; j++) {
                        ret.push_back(i & 0xFFFF);
                        i >>= 16;
                    }
                    break;
            }
            return ret;
            
        }
        vector <unsigned long> numInterpret(string buffer) {
            vector <unsigned long> ret;
            unsigned int i;
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) {
                        bool gud = true;
                        switch (buffer[1]) {
                            case 'x':
                                i = stoll(buffer.substr(2), nullptr, 16);
                                ret.push_back(i & 0xFFFF);
                                break;
                            case 'o':
                                i = stoll(buffer.substr(2), nullptr, 8);
                                ret.push_back(i & 0xFFFF);
                                break;
                            case 'b':
                                i = stoll(buffer.substr(2), nullptr, 2);
                                ret.push_back(i & 0xFFFF);
                                break;
                            case 'c':
                                ret.push_back(buffer[2] & 0xFFFF);
                                break;
                            case 's':
                                for (int i = 3; i < buffer.length() - 1; i++) {
                                    ret.push_back(buffer[i] & 0xFFFF);
                                }
                                break;
                            default:
                                gud = false;
                                break;
                        }
                        if (gud) break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    i = stoll(buffer, nullptr, 10);
                    ret.push_back(i & 0xFFFF);
                    break;
            }
            return ret;
            
        }
        unsigned long long numInterpretInt(string buffer) {
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) switch (buffer[1]) {
                        case 'x':
                            return stoll(buffer.substr(2), nullptr, 16);
                        case 'o':
                            return stoll(buffer.substr(2), nullptr, 8);
                        case 'b':
                            return stoll(buffer.substr(2), nullptr, 2);
                        case 'c':
                            return buffer[2];
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    return stoll(buffer, nullptr, 10);
            }
            throw RuntimeError("Not a number.");
        }
        unordered_map<string,unsigned int> labels;
        unordered_map<string,Definition> definitions;
        vector <unsigned long> exp;
        unsigned int pos = 0;
        bool inString = false;
        string str_buf_curr;
        unsigned long long clock;
        vector <string> callstack;
        void print_info(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_DARK_CYAN << round(getTime() - clock) / 1000 << " INFO " << info << COLOR_RESET << endl;
        }
        void print_error(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_RED << round(getTime() - clock) / 1000 << " ERROR " << info << COLOR_RESET << endl;
        }
        void print_warning(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_YELLOW << round(getTime() - clock) / 1000 << " WARN " << info << COLOR_RESET << endl;
        }
        void print_debug(string info) {
            if (!verbose) return;
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_GREEN << round(getTime() - clock) / 1000 << " DEBUG " << info << COLOR_RESET << endl;
        }
        void print_fatal_error(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_WHITE COLOR_BACKGROUND_DARK_RED << round(getTime() - clock) / 1000 << " FATAL " << info << COLOR_RESET << endl;
        }
    private:
        bool inComment = false;
        unsigned char inst = 0;
        unsigned int posix = 0;
        unsigned int posF = 0;
        string buffer_def;
        string buffer__def;
    };
    template <uint8_t bits>
    class Assembler : protected OldAssembler<bits> {
    public:
        using OldAssembler::addrlen;
        using OldAssembler::consts;
        using OldAssembler::verbose;
        vector <unsigned char> build(string asm_code);
        void reset() {
            uncommentedcode.clear();
            codewdefs.reset();
            codewconsts.reset();
            codewlabels.reset();
            definitions.clear();
            labels.clear();
            includedalready.clear();
            in_org = false;
            inst = NONE;
            posix = 0;
            exec = 0;
            add[0] = 0;
            add[1] = 0;
            sub[0] = 0;
            sub[1] = 0;
            mul[0] = 0;
            mul[1] = 0;
            doingop = false;
        }
        Assembler() {}
    protected:
        void uncomment(string asm_code);
        void stage1tostage2asm();
        void stage2tostage3asm();
        void stage3tostage4asm();
        void stage4tofinalasm();
        vector <string> includedalready;
        vector <string> uncommentedcode; //stage 1 code
        AsmCode_Obj<unsigned char> codewdefs; //stage 2 code
        AsmCode_Obj<unsigned char> codewconsts; //stage 3 code
        AsmCode_Obj<unsigned char> codewlabels; //stage 4 code
        AsmCode_Obj<unsigned char> finishedcode; // final code
        unordered_map <string,string> indexedfilenames;
        char combine = 0;
        char add[2] = {0};
        char sub[2] = {0};
        char mul[2] = {0};
        bool doingop = false;
        //index all filenames in the program's directory and subdirectories
        void index_filenames() {
            if (indexedfilenames.size() == 0) {
                for (const auto& entry : fs::recursive_directory_iterator("./")) {
                    if (!entry.is_directory()) {
                        if (isIn(indexedfilenames,entry.path().filename().string())) continue;
                        indexedfilenames[entry.path().filename().string()] = entry.path().string();
                    }
                }
            }
        }
        void throw_err_comp(string message) {
            print_error(message + " --- " + callstack.back());
            print_debug("Callstack (from first call):");
            for (string s : callstack) {
                print_debug(s);
            }
            print_debug("End of callstack.");
            print_fatal_error("Cannot continue build process.");
            throw RuntimeError(message);
        }
        void throw_err(string message) {
            print_error(message + " --- " + callstack.back());
            print_debug("Callstack (from first call):");
            for (string s : callstack) {
                print_debug(s);
            }
            print_debug("End of callstack.");
            throw RuntimeError(message);
        }
        void include_filename(string filename) {
            index_filenames();
            if (!isIn(includedalready, filename)) {
                includedalready.push_back(filename);
                std::ifstream file (indexedfilenames[filename]);
                if (file.is_open()) {
                    string file_f;
                    while (true) {
                        char c = file.get();
                        if (file.eof()) break;
                        file_f += c;
                    }
                    file.close();
                    callstack.push_back(split(filename, '.')[0]);
                    print_debug("Including file: " + filename);
                    uncomment(file_f);
                    stage1tostage2asm();
                    print_debug("Finished including file: " + filename);
                    callstack.pop_back();
                } else {
                    throw_err_comp("Cannot open file for include " + filename);
                }
            }
        }
        void do_def(string def,string curr_def) {
            if (combine == 1) {
                CodeSegment f = codewdefs.code.back();
                codewdefs.code.pop_back();
                combine = 0;
                if (f.is_string) do_def(f.code + def,curr_def);
                else do_def(std::to_string((int)f.value) + def,curr_def);
            } else if (combine == 2) {
                size_t s = codewdefs.code.size();
                CodeSegment f = codewdefs.code.back();
                codewdefs.code.pop_back();
                combine = 0;
                do_def(def,curr_def);
                if (s >= codewdefs.code.size()) {
                    vector<CodeSegment> new_code;
                    while ((codewdefs.code.size() + 1) > s) {
                        new_code.push_back(codewdefs.code.back());
                        codewdefs.code.pop_back();
                    }
                    CodeSegment v = new_code.front();
                    new_code.erase(new_code.begin());
                    if (f.is_string) {
                        if (v.is_string) {
                            do_def(f.code + v.code,curr_def);
                        } else {
                            do_def(f.code + std::to_string((int)v.value),curr_def);
                        }
                    } else {
                        if (v.is_string) {
                            do_def(std::to_string((int)f.value) + v.code,curr_def);
                        } else {
                            do_def(std::to_string((int)f.value) + std::to_string((int)v.value),curr_def);
                        }
                    }
                    codewdefs.code.insert(codewdefs.code.end(),new_code.begin(),new_code.end());
                } else {
                    codewdefs.code.push_back(f);
                    combine = 2;
                }
            } else if (isIn(definitions,def)) {
                print_debug("Found definition call for " + def);
                if (def == curr_def) {
                    throw_err_comp("Cannot have a recursive definition call for " + def);
                }
                callstack.push_back(def);
                uncomment(definitions[def].def);
                stage1tostage2asm();
                callstack.pop_back();
                print_debug("Ended definition call for " + def);
            } else {
                codewdefs.code.push_back(def);
            }
        }
        enum {
            NONE,
            DEF, //handled
            IF, //unimplemented
            IFDEF, //handled
            IFNDEF, //handled
            UNDEF, //handled
            FILL, //handled
            FILLEMP, //handled
            INCLUDE, //handled
            ERROR //handled
        } inst;
        bool in_org = false;
        unsigned long long posix = 0;
        temp_inst_t temp_inst[3];
        unsigned long long exec = 0;
    };
    class Assembler<8> : protected OldAssembler<8> {
    public:
        using OldAssembler::addrlen;
        using OldAssembler::consts;
        using OldAssembler::verbose;
        vector <unsigned char> build(string asm_code) {
            try {
                clock = getTime();
                srand(time(NULL));
                callstack.push_back("__main__0x" + to_string(FULL_RAND));
                print_info("Started build for main");
                uncomment(asm_code);
                stage1tostage2asm();
                stage2tostage3asm();
                stage3tostage4asm();
                stage4tofinalasm();
                vector<unsigned char> ret;
                for (auto const& i : finishedcode.code) {
                    ret.insert(ret.end(),i.value);
                }
                print_info("Finished build for main");
                callstack.pop_back();
                return ret;
            } catch (std::exception e) {
                print_fatal_error(e.what());
                throw e;
            }
        };
        void reset() {
            uncommentedcode.clear();
            codewdefs.reset();
            codewconsts.reset();
            codewlabels.reset();
            definitions.clear();
            labels.clear();
            includedalready.clear();
            in_org = false;
            inst = NONE;
            posix = 0;
            exec = 0;
            add[0] = 0;
            add[1] = 0;
            sub[0] = 0;
            sub[1] = 0;
            mul[0] = 0;
            mul[1] = 0;
            doingop = false;
        }
        Assembler() {}
    protected:
        void uncomment(string asm_code) {
            string buffer;
            bool inComment = false;
            asm_code+= " ";
            print_debug("Beginning build stage 1 for callstack " + callstack.back());
            for (char c : asm_code) switch (c) {
                case ' ':
                case '\n':
                case '\t':
                    if (buffer.length() != 0) {
                        if (inComment && buffer == "*/") {
                            inComment = false;
                            buffer.clear();
                        } else if (!inComment && buffer == "/*") {
                            inComment = true;
                            buffer.clear();
                        } else if (!inComment) {
                            if (count(buffer, '"') == 0 
                            || (count(buffer,'"') > 0 && !startsWith(buffer,"0s")) 
                            || count(buffer,'"') % 2 == 0) {
                                uncommentedcode.push_back(buffer);
                                buffer.clear();
                            } else if (count(buffer,'"') % 2 == 1) {
                                buffer.push_back(c);
                            }
                        } else {
                            buffer.clear();
                        }
                    }
                    break;
                default:
                    buffer += c;
                    break;
            }
            print_debug("Finishing build stage 1 for callstack " + callstack.back());
        }
        void stage1tostage2asm() {
            vector<string> new_com = uncommentedcode;
            uncommentedcode.clear();
            print_debug("Beginning build stage 2 on callstack " + callstack.back());
            for (string s : new_com) {
                if (s == "__RANDOM_ID__" && posix == 1) {
                    std::stringstream st;
                    st << "0x" << std::hex << FULL_RAND;
                    s = st.str();
                }
                if (exec > 0) {
                    if (s == ".def") exec++;
                    else if (s == ".if") exec++;
                    else if (s == ".ifdef") exec++;
                    else if (s == ".ifndef") exec++;
                    else if (s == ".end") exec--;
                    else if (s == ".else" && exec == 1) exec--;
                }
                if (exec == 0 || (exec == 1 && (s == ".elif" || s == ".elifdef" || s == ".elifndef"))) {
                    bool insf = false;
                    if (s[0] == '.' && inst == NONE) {
                        if (s == ".emp") inst = FILLEMP;
                        else if (s == ".fill") inst = FILL;
                        else if (s == ".def") inst = DEF;
                        else if (s == ".if") inst = IF;
                        else if (s == ".undef") inst = UNDEF;
                        else if (s == ".ifdef") inst = IFDEF;
                        else if (s == ".elif") inst = IF;
                        else if (s == ".elifdef") inst = IFDEF;
                        else if (s == ".elifndef") inst = IFNDEF;
                        else if (s == ".error") inst = ERROR;
                        else if (s == ".include") inst = INCLUDE;
                        else insf = true;
                        if (!insf) posix = 0;
                    } else insf = true;
                    if (insf) switch(inst) {
                        case FILLEMP:
                            if (true) {
                                unsigned long long a = numInterpretInt(s);
                                for (unsigned int i = 0; i < a; i++) codewdefs.code.push_back(0);
                                print_debug("Filling " + to_string(a) + " bytes with 0.");
                                inst = NONE;
                                break;
                            }
                        case FILL:
                            if (posix == 0) {
                                temp_inst[0].i = numInterpretInt(s);
                                posix = 1;
                            } else {
                                unsigned long long a = numInterpretInt(s);
                                for (unsigned long long i = 0; i < a; i++) codewdefs.code.push_back(temp_inst[0].i);
                                print_debug("Filling " + to_string(a) + " bytes with " + to_string(temp_inst[0].i));
                                posix = 0;
                                inst = NONE;
                            }
                            break;
                        case DEF:
                            if (posix == 0) {
                                temp_inst[0].s = s;
                                temp_inst[1].s = string("");
                                posix = 1;
                            } else {
                                if (s == ".end") posix--;
                                else if (s == ".def") posix++;
                                else if (s == ".if") posix++;
                                else if (s == ".ifdef") posix++;
                                else if (s == ".ifndef") posix++;
                                if (posix == 0) {
                                    temp_inst[1].s.pop_back();
                                    definitions[temp_inst[0].s] = Definition(temp_inst[1].s);
                                    print_debug("Created definition for " + temp_inst[0].s);
                                    inst = NONE;
                                } else {
                                    temp_inst[1].s += s + " ";
                                }
                            }
                            break;
                        case UNDEF:
                            if (definitions.find(s) != definitions.end()) {
                                definitions.erase(s);
                                print_debug("Removed definition for " + s);
                            } else {
                                print_warning("Attempted removal of definition for " + s + " but it does not exist.");
                            }
                            inst = NONE;
                            break;
                        case IF:
                            if (posix == 0) {
                                temp_inst[0].s = s + " ";
                                posix++;
                            } else if (s == ".then") {
                                posix = 0;
                                inst = NONE;
                                temp_inst[0].s.pop_back();
                                print_warning("Use of .if but its not implemented in callstack " + callstack.back());
                                //unimplemented
                            } else temp_inst[0].s += s + " ";
                            break;
                        case IFDEF:
                            if (posix == 0) {
                                temp_inst[0].s = s + " ";
                                posix++;
                            } else if (s == ".then") {
                                posix = 0;
                                inst = NONE;
                                temp_inst[0].s.pop_back();
                                if (definitions.find(temp_inst[0].s) == definitions.end()) {
                                    exec++;
                                    print_debug(".ifdef evaluation for " + temp_inst[0].s + " is false.");
                                } else {
                                    print_debug(".ifdef evaluation for " + temp_inst[0].s + " is true.");
                                }
                            } else temp_inst[0].s += s + " ";
                            break;
                        case IFNDEF:
                            if (posix == 0) {
                                temp_inst[0].s = s + " ";
                                posix++;
                            } else if (s == ".then") {
                                posix = 0;
                                inst = NONE;
                                temp_inst[0].s.pop_back();
                                if (definitions.find(temp_inst[0].s) != definitions.end()) {
                                    exec++;
                                    print_debug(".ifndef evaluation for " + temp_inst[0].s + " is false.");
                                } else {
                                    print_debug(".ifndef evaluation for " + temp_inst[0].s + " is true.");
                                }
                            } else temp_inst[0].s += s + " ";
                            break;
                        case NONE:
                            if (s == "##") {
                                combine = 2;
                            } else if (s == "#"){
                                combine = 1;
                            } else do_def(s,callstack.back());
                            break;
                        case INCLUDE:
                            if (posix == 0) {
                                temp_inst[0].s = slice(s, 1);
                                if (count((temp_inst[0].s), '"') == 1) {
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-1);
                                    include_filename(temp_inst[0].s);
                                } else posix++;
                                
                            } else {
                                temp_inst[0].s += s + " ";
                                if (count((temp_inst[0].s), '"') == 1) {
                                    posix--;
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-2);
                                    include_filename(temp_inst[0].s);
                                }
                            }
                            break;
                        case ERROR:
                            if (posix == 0) {
                                temp_inst[0].s = slice(s, 1);
                                if (count((temp_inst[0].s), '"') == 2) {
                                    posix--;
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-1);
                                    throw_err_comp("Error directive: " + temp_inst[0].s);
                                }
                                posix++;
                            } else {
                                temp_inst[0].s += s + " ";
                                if (count((temp_inst[0].s), '"') == 2) {
                                    posix--;
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-2);
                                    throw_err_comp("Error directive: " + temp_inst[0].s);
                                }
                            }
                            break;
                    }
                }
            }
            print_debug("Finishing build stage 2 on callstack " + callstack.back());
        }
        void stage2tostage3asm() {
            AsmCode_Obj<unsigned char> new_com = codewdefs;
            codewdefs.reset();
            print_debug("Starting build stage 3 on callstack " + callstack.back());
            for (const CodeSegment<unsigned char>& i : new_com.code) {
                if (!i.is_string) {
                    codewconsts.code.push_back(i);
                } else if (isIn(consts,i.code)) {
                    codewconsts.code.push_back(consts[i.code]);
                } else switch(i.code[0]) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if (doingop) {
                            if (add[0] > 0) {
                                for (auto& a : add) if (a > 4) {
                                    print_warning("Attempted to do an add operation of more than 4 bytes. 4 bytes will be used.");
                                    a = 4;
                                }
                                int v = max(add[0], add[1]);
                                unsigned int a = 0;
                                unsigned int b = numInterpretInt(i.code);
                                for (int i = 0; i < add[0]; i++) {
                                    a += codewconsts.code.back().value * pow(256, add[0]-i);
                                    codewconsts.code.pop_back();
                                }
                                vector<unsigned char> ve;
                                for (int i = 0; i < v; i++) {
                                    ve.push_back((a + b) >> (8*i));
                                }
                                codewconsts.code.insert(codewconsts.code.end(),ve.begin(),ve.end());
                                doingop = false;
                                add[0] = 0;
                                add[1] = 0;
                            } else if (sub[0] > 0) {
                                for (auto& a : sub) if (a > 4) {
                                    print_warning("Attempted to do a sub operation of more than 4 bytes. 4 bytes will be used.");
                                    a = 4;
                                }
                                int v = max(sub[0], sub[1]);
                                unsigned int a = 0;
                                unsigned int b = numInterpretInt(i.code);
                                for (int i = 0; i < sub[0]; i++) {
                                    a += codewconsts.code.back().value * pow(256, sub[0]-i);
                                    codewconsts.code.pop_back();
                                }
                                vector<unsigned char> ve;
                                for (int i = 0; i < v; i++) {
                                    ve.push_back((a - b) >> (8*i));
                                }
                                codewconsts.code.insert(codewconsts.code.end(),ve.begin(),ve.end());
                                doingop = false;
                                sub[0] = 0;
                                sub[1] = 0;
                            } else if (mul[0] > 0) {
                                for (auto& a : mul) if (a > 4) {
                                    print_warning("Attempted to do a mul operation of more than 4 bytes. 4 bytes will be used.");
                                    a = 4;
                                }
                                int v = max(mul[0], mul[1]);
                                unsigned int a = 0;
                                unsigned int b = numInterpretInt(i.code);
                                for (int i = 0; i < mul[0]; i++) {
                                    a += codewconsts.code.back().value * pow(256, mul[0]-i);
                                    codewconsts.code.pop_back();
                                }
                                vector<unsigned char> ve;
                                for (int i = 0; i < v; i++) {
                                    ve.push_back((a * b) >> (8*i));
                                }
                                codewconsts.code.insert(codewconsts.code.end(),ve.begin(),ve.end());
                            }
                        } else {
                            try {
                                vector<unsigned char> v = numInterpret(i.code);
                                codewconsts.code.insert(codewconsts.code.end(),v.begin(),v.end());
                            } catch (std::exception e) {
                                print_debug((string)"Failed to interpret number:" + e.what());
                                throw_err_comp("Error interpreting number: " + i.code);
                            }
                        }
                        break;
                    case '+':
                        if (true) {
                            string s = i.code.substr(1);
                            int i = 0;
                            for (auto& sv : split(s, "_")) {
                                add[i] = numInterpretInt(sv);
                                i++;
                            }
                        }
                        break;
                    case '-':
                        if (true) {
                            string s = i.code.substr(1);
                            int i = 0;
                            for (auto& sv : split(s, "_")) {
                                sub[i] = numInterpretInt(sv);
                                i++;
                            }
                        }
                        break;
                    case '*':
                        if (true) {
                            string s = i.code.substr(1);
                            int i = 0;
                            for (auto& sv : split(s, "_")) {
                                mul[i] = numInterpretInt(sv);
                                i++;
                            }
                        }
                        break;
                    default:
                        codewconsts.code.push_back(i.code);
                }
            }
            print_debug("Finishing build stage 3 on callstack " + callstack.back());
        }
        void stage3tostage4asm() {
            AsmCode_Obj<unsigned char> new_com = codewconsts;
            codewconsts.code.clear();
            print_debug("Starting build stage 4 on callstack " + callstack.back());
            for (const CodeSegment<unsigned char>& i : new_com.code) {
                if (i.is_string) {
                    if (i.code.length() == 0) continue;
                    if (in_org) {
                        in_org = false;
                        try {
                            codewconsts.pos = numInterpretInt(i.code);

                        } catch (std::exception e) {
                            print_debug((string)"Failed to interpret number:" + e.what());
                            throw_err_comp("Error interpreting number: " + i.code);
                        }
                    }
                    else if (i.code == ".org") {
                        in_org = true;
                    } else if (i.code[0] == ':') {
                        if (isIn(labels,i.code)) {
                            throw_err("Cannot redefine label " + i.code);
                        } else {
                            labels[i.code.substr(1)] = codewconsts.pos;
                        }
                    } else {
                        print_debug("Found a reference to a possible label: " + i.code);
                        codewconsts.pos+=3;
                        codewlabels.code.push_back(i);
                    }
                } else {
                    codewlabels.code.push_back(i);
                    ++codewconsts.pos;
                }
            }
            print_debug("Finishing build stage 4 on callstack " + callstack.back());
        }
        void stage4tofinalasm() {
            AsmCode_Obj<unsigned char> new_com = codewlabels;
            codewlabels.code.clear();
            print_debug("Finishing build on callstack " + callstack.back());
            for (const CodeSegment<unsigned char>& i : new_com.code) {
                if (i.is_string) {
                    if (isIn(labels,i.code)) {
                        unsigned int pos = labels[i.code];
                        for (int i = 0; i < addrlen; i++) {
                            finishedcode.code.push_back(pos & 0xFF);
                            pos >>= 8;
                        }
                    } else throw_err_comp("Uninterpreted string found while finalizing \"" + i.code + "\"");
                } else {
                    finishedcode.code.push_back(i);
                }
            }
            print_debug("Finished build on callstack " + callstack.back());
        }
        vector <string> includedalready;
        vector <string> uncommentedcode; //stage 1 code
        AsmCode_Obj<unsigned char> codewdefs; //stage 2 code
        AsmCode_Obj<unsigned char> codewconsts; //stage 3 code
        AsmCode_Obj<unsigned char> codewlabels; //stage 4 code
        AsmCode_Obj<unsigned char> finishedcode; // final code
        unordered_map <string,string> indexedfilenames;
        char combine = 0;
        char add[2] = {0};
        char sub[2] = {0};
        char mul[2] = {0};
        bool doingop = false;
        //index all filenames in the program's directory and subdirectories
        void index_filenames() {
            if (indexedfilenames.size() == 0) {
                for (const auto& entry : fs::recursive_directory_iterator("./")) {
                    if (!entry.is_directory()) {
                        if (isIn(indexedfilenames,entry.path().filename().string())) continue;
                        indexedfilenames[entry.path().filename().string()] = entry.path().string();
                    }
                }
            }
        }
        void throw_err_comp(string message) {
            print_error(message + " --- " + callstack.back());
            print_debug("Callstack (from first call):");
            for (string s : callstack) {
                print_debug(s);
            }
            print_debug("End of callstack.");
            print_fatal_error("Cannot continue build process.");
            throw RuntimeError(message);
        }
        void throw_err(string message) {
            print_error(message + " --- " + callstack.back());
            print_debug("Callstack (from first call):");
            for (string s : callstack) {
                print_debug(s);
            }
            print_debug("End of callstack.");
            throw RuntimeError(message);
        }
        void include_filename(string filename) {
            index_filenames();
            if (!isIn(includedalready, filename)) {
                includedalready.push_back(filename);
                std::ifstream file (indexedfilenames[filename]);
                if (file.is_open()) {
                    string file_f;
                    while (true) {
                        char c = file.get();
                        if (file.eof()) break;
                        file_f += c;
                    }
                    file.close();
                    callstack.push_back(split(filename, '.')[0]);
                    print_debug("Including file: " + filename);
                    uncomment(file_f);
                    stage1tostage2asm();
                    print_debug("Finished including file: " + filename);
                    callstack.pop_back();
                } else {
                    throw_err_comp("Cannot open file for include " + filename);
                }
            }
        }
        void do_def(string def,string curr_def) {
            if (combine == 1) {
                CodeSegment<unsigned char> f = codewdefs.code.back();
                codewdefs.code.pop_back();
                combine = 0;
                if (f.is_string) do_def(f.code + def,curr_def);
                else do_def(std::to_string((int)f.value) + def,curr_def);
            } else if (combine == 2) {
                size_t s = codewdefs.code.size();
                CodeSegment<unsigned char> f = codewdefs.code.back();
                codewdefs.code.pop_back();
                combine = 0;
                do_def(def,curr_def);
                if (s >= codewdefs.code.size()) {
                    vector<CodeSegment<unsigned char>> new_code;
                    while ((codewdefs.code.size() + 1) > s) {
                        new_code.push_back(codewdefs.code.back());
                        codewdefs.code.pop_back();
                    }
                    CodeSegment<unsigned char> v = new_code.front();
                    new_code.erase(new_code.begin());
                    if (f.is_string) {
                        if (v.is_string) {
                            do_def(f.code + v.code,curr_def);
                        } else {
                            do_def(f.code + std::to_string((int)v.value),curr_def);
                        }
                    } else {
                        if (v.is_string) {
                            do_def(std::to_string((int)f.value) + v.code,curr_def);
                        } else {
                            do_def(std::to_string((int)f.value) + std::to_string((int)v.value),curr_def);
                        }
                    }
                    codewdefs.code.insert(codewdefs.code.end(),new_code.begin(),new_code.end());
                } else {
                    codewdefs.code.push_back(f);
                    combine = 2;
                }
            } else if (isIn(definitions,def)) {
                print_debug("Found definition call for " + def);
                if (def == curr_def) {
                    throw_err_comp("Cannot have a recursive definition call for " + def);
                }
                callstack.push_back(def);
                uncomment(definitions[def].def);
                stage1tostage2asm();
                callstack.pop_back();
                print_debug("Ended definition call for " + def);
            } else {
                codewdefs.code.push_back(def);
            }
        }
        enum {
            NONE,
            DEF, //handled
            IF, //unimplemented
            IFDEF, //handled
            IFNDEF, //handled
            UNDEF, //handled
            FILL, //handled
            FILLEMP, //handled
            INCLUDE, //handled
            ERROR //handled
        } inst;
        bool in_org = false;
        unsigned long long posix = 0;
        temp_inst_t temp_inst[3];
        unsigned long long exec = 0;
    };
    class Assembler<16> : protected OldAssembler<16> {
    public:
        using OldAssembler::addrlen;
        using OldAssembler::consts;
        using OldAssembler::verbose;
        vector <unsigned short> build(string asm_code) {
            try {
                clock = getTime();
                srand(time(NULL));
                callstack.push_back("__main__0x" + to_string(FULL_RAND));
                print_info("Started build for main");
                uncomment(asm_code);
                stage1tostage2asm();
                stage2tostage3asm();
                stage3tostage4asm();
                stage4tofinalasm();
                vector<unsigned short> ret;
                for (auto const& i : finishedcode.code) {
                    ret.insert(ret.end(),i.value);
                }
                print_info("Finished build for main");
                callstack.pop_back();
                return ret;
            } catch (std::exception e) {
                print_fatal_error(e.what());
                throw e;
            }
        };
        void reset() {
            uncommentedcode.clear();
            codewdefs.reset();
            codewconsts.reset();
            codewlabels.reset();
            definitions.clear();
            labels.clear();
            includedalready.clear();
            in_org = false;
            inst = NONE;
            posix = 0;
            exec = 0;
            add[0] = 0;
            add[1] = 0;
            sub[0] = 0;
            sub[1] = 0;
            mul[0] = 0;
            mul[1] = 0;
            doingop = false;
        }
        Assembler() {}
    protected:
        void uncomment(string asm_code) {
            string buffer;
            bool inComment = false;
            asm_code+= " ";
            print_debug("Beginning build stage 1 for callstack " + callstack.back());
            for (char c : asm_code) switch (c) {
                case ' ':
                case '\n':
                case '\t':
                    if (buffer.length() != 0) {
                        if (inComment && buffer == "*/") {
                            inComment = false;
                            buffer.clear();
                        } else if (!inComment && buffer == "/*") {
                            inComment = true;
                            buffer.clear();
                        } else if (!inComment) {
                            if (count(buffer, '"') == 0 
                            || (count(buffer,'"') > 0 && !startsWith(buffer,"0s")) 
                            || count(buffer,'"') % 2 == 0) {
                                uncommentedcode.push_back(buffer);
                                buffer.clear();
                            } else if (count(buffer,'"') % 2 == 1) {
                                buffer.push_back(c);
                            }
                        } else {
                            buffer.clear();
                        }
                    }
                    break;
                default:
                    buffer += c;
                    break;
            }
            print_debug("Finishing build stage 1 for callstack " + callstack.back());
        }
        void stage1tostage2asm() {
            vector<string> new_com = uncommentedcode;
            uncommentedcode.clear();
            print_debug("Beginning build stage 2 on callstack " + callstack.back());
            for (string s : new_com) {
                if (s == "__RANDOM_ID__" && posix == 1) {
                    std::stringstream st;
                    st << "0x" << std::hex << FULL_RAND;
                    s = st.str();
                }
                if (exec > 0) {
                    if (s == ".def") exec++;
                    else if (s == ".if") exec++;
                    else if (s == ".ifdef") exec++;
                    else if (s == ".ifndef") exec++;
                    else if (s == ".end") exec--;
                    else if (s == ".else" && exec == 1) exec--;
                }
                if (exec == 0 || (exec == 1 && (s == ".elif" || s == ".elifdef" || s == ".elifndef"))) {
                    bool insf = false;
                    if (s[0] == '.' && inst == NONE) {
                        if (s == ".emp") inst = FILLEMP;
                        else if (s == ".fill") inst = FILL;
                        else if (s == ".def") inst = DEF;
                        else if (s == ".if") inst = IF;
                        else if (s == ".undef") inst = UNDEF;
                        else if (s == ".ifdef") inst = IFDEF;
                        else if (s == ".elif") inst = IF;
                        else if (s == ".elifdef") inst = IFDEF;
                        else if (s == ".elifndef") inst = IFNDEF;
                        else if (s == ".error") inst = ERROR;
                        else if (s == ".include") inst = INCLUDE;
                        else insf = true;
                        if (!insf) posix = 0;
                    } else insf = true;
                    if (insf) switch(inst) {
                        case FILLEMP:
                            if (true) {
                                unsigned long long a = numInterpretInt(s);
                                for (unsigned int i = 0; i < a; i++) codewdefs.code.push_back(0);
                                print_debug("Filling " + to_string(a) + " bytes with 0.");
                                inst = NONE;
                                break;
                            }
                        case FILL:
                            if (posix == 0) {
                                temp_inst[0].i = numInterpretInt(s);
                                posix = 1;
                            } else {
                                unsigned long long a = numInterpretInt(s);
                                for (unsigned long long i = 0; i < a; i++) codewdefs.code.push_back(temp_inst[0].i);
                                print_debug("Filling " + to_string(a) + " bytes with " + to_string(temp_inst[0].i));
                                posix = 0;
                                inst = NONE;
                            }
                            break;
                        case DEF:
                            if (posix == 0) {
                                temp_inst[0].s = s;
                                temp_inst[1].s = string("");
                                posix = 1;
                            } else {
                                if (s == ".end") posix--;
                                else if (s == ".def") posix++;
                                else if (s == ".if") posix++;
                                else if (s == ".ifdef") posix++;
                                else if (s == ".ifndef") posix++;
                                if (posix == 0) {
                                    temp_inst[1].s.pop_back();
                                    definitions[temp_inst[0].s] = Definition(temp_inst[1].s);
                                    print_debug("Created definition for " + temp_inst[0].s);
                                    inst = NONE;
                                } else {
                                    temp_inst[1].s += s + " ";
                                }
                            }
                            break;
                        case UNDEF:
                            if (definitions.find(s) != definitions.end()) {
                                definitions.erase(s);
                                print_debug("Removed definition for " + s);
                            } else {
                                print_warning("Attempted removal of definition for " + s + " but it does not exist.");
                            }
                            inst = NONE;
                            break;
                        case IF:
                            if (posix == 0) {
                                temp_inst[0].s = s + " ";
                                posix++;
                            } else if (s == ".then") {
                                posix = 0;
                                inst = NONE;
                                temp_inst[0].s.pop_back();
                                print_warning("Use of .if but its not implemented in callstack " + callstack.back());
                                //unimplemented
                            } else temp_inst[0].s += s + " ";
                            break;
                        case IFDEF:
                            if (posix == 0) {
                                temp_inst[0].s = s + " ";
                                posix++;
                            } else if (s == ".then") {
                                posix = 0;
                                inst = NONE;
                                temp_inst[0].s.pop_back();
                                if (definitions.find(temp_inst[0].s) == definitions.end()) {
                                    exec++;
                                    print_debug(".ifdef evaluation for " + temp_inst[0].s + " is false.");
                                } else {
                                    print_debug(".ifdef evaluation for " + temp_inst[0].s + " is true.");
                                }
                            } else temp_inst[0].s += s + " ";
                            break;
                        case IFNDEF:
                            if (posix == 0) {
                                temp_inst[0].s = s + " ";
                                posix++;
                            } else if (s == ".then") {
                                posix = 0;
                                inst = NONE;
                                temp_inst[0].s.pop_back();
                                if (definitions.find(temp_inst[0].s) != definitions.end()) {
                                    exec++;
                                    print_debug(".ifndef evaluation for " + temp_inst[0].s + " is false.");
                                } else {
                                    print_debug(".ifndef evaluation for " + temp_inst[0].s + " is true.");
                                }
                            } else temp_inst[0].s += s + " ";
                            break;
                        case NONE:
                            if (s == "##") {
                                combine = 2;
                            } else if (s == "#"){
                                combine = 1;
                            } else do_def(s,callstack.back());
                            break;
                        case INCLUDE:
                            if (posix == 0) {
                                temp_inst[0].s = slice(s, 1);
                                if (count((temp_inst[0].s), '"') == 1) {
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-1);
                                    include_filename(temp_inst[0].s);
                                } else posix++;
                                
                            } else {
                                temp_inst[0].s += s + " ";
                                if (count((temp_inst[0].s), '"') == 1) {
                                    posix--;
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-2);
                                    include_filename(temp_inst[0].s);
                                }
                            }
                            break;
                        case ERROR:
                            if (posix == 0) {
                                temp_inst[0].s = slice(s, 1);
                                if (count((temp_inst[0].s), '"') == 2) {
                                    posix--;
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-1);
                                    throw_err_comp("Error directive: " + temp_inst[0].s);
                                }
                                posix++;
                            } else {
                                temp_inst[0].s += s + " ";
                                if (count((temp_inst[0].s), '"') == 2) {
                                    posix--;
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-2);
                                    throw_err_comp("Error directive: " + temp_inst[0].s);
                                }
                            }
                            break;
                    }
                }
            }
            print_debug("Finishing build stage 2 on callstack " + callstack.back());
        }
        void stage2tostage3asm() {
            AsmCode_Obj<unsigned short> new_com = codewdefs;
            codewdefs.reset();
            print_debug("Starting build stage 3 on callstack " + callstack.back());
            for (const CodeSegment<unsigned short>& i : new_com.code) {
                if (!i.is_string) {
                    codewconsts.code.push_back(i);
                } else if (isIn(consts,i.code)) {
                    codewconsts.code.push_back(consts[i.code]);
                } else switch(i.code[0]) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if (doingop) {
                            if (add[0] > 0) {
                                for (auto& a : add) if (a > 4) {
                                    print_warning("Attempted to do an add operation of more than 4 bytes. 4 bytes will be used.");
                                    a = 4;
                                }
                                int v = max(add[0], add[1]);
                                unsigned int a = 0;
                                unsigned int b = numInterpretInt(i.code);
                                for (int i = 0; i < add[0]; i++) {
                                    a += codewconsts.code.back().value * pow(256, add[0]-i);
                                    codewconsts.code.pop_back();
                                }
                                vector<unsigned short> ve;
                                for (int i = 0; i < v; i++) {
                                    ve.push_back((a + b) >> (8*i));
                                }
                                codewconsts.code.insert(codewconsts.code.end(),ve.begin(),ve.end());
                                doingop = false;
                                add[0] = 0;
                                add[1] = 0;
                            } else if (sub[0] > 0) {
                                for (auto& a : sub) if (a > 4) {
                                    print_warning("Attempted to do a sub operation of more than 4 bytes. 4 bytes will be used.");
                                    a = 4;
                                }
                                int v = max(sub[0], sub[1]);
                                unsigned int a = 0;
                                unsigned int b = numInterpretInt(i.code);
                                for (int i = 0; i < sub[0]; i++) {
                                    a += codewconsts.code.back().value * pow(256, sub[0]-i);
                                    codewconsts.code.pop_back();
                                }
                                vector<unsigned short> ve;
                                for (int i = 0; i < v; i++) {
                                    ve.push_back((a - b) >> (8*i));
                                }
                                codewconsts.code.insert(codewconsts.code.end(),ve.begin(),ve.end());
                                doingop = false;
                                sub[0] = 0;
                                sub[1] = 0;
                            } else if (mul[0] > 0) {
                                for (auto& a : mul) if (a > 4) {
                                    print_warning("Attempted to do a mul operation of more than 4 bytes. 4 bytes will be used.");
                                    a = 4;
                                }
                                int v = max(mul[0], mul[1]);
                                unsigned int a = 0;
                                unsigned int b = numInterpretInt(i.code);
                                for (int i = 0; i < mul[0]; i++) {
                                    a += codewconsts.code.back().value * pow(256, mul[0]-i);
                                    codewconsts.code.pop_back();
                                }
                                vector<unsigned short> ve;
                                for (int i = 0; i < v; i++) {
                                    ve.push_back((a * b) >> (8*i));
                                }
                                codewconsts.code.insert(codewconsts.code.end(),ve.begin(),ve.end());
                            }
                        } else {
                            try {
                                vector<unsigned short> v = numInterpret(i.code);
                                codewconsts.code.insert(codewconsts.code.end(),v.begin(),v.end());
                            } catch (std::exception e) {
                                print_debug((string)"Failed to interpret number:" + e.what());
                                throw_err_comp("Error interpreting number: " + i.code);
                            }
                        }
                        break;
                    case '+':
                        if (true) {
                            string s = i.code.substr(1);
                            int i = 0;
                            for (auto& sv : split(s, "_")) {
                                add[i] = numInterpretInt(sv);
                                i++;
                            }
                        }
                        break;
                    case '-':
                        if (true) {
                            string s = i.code.substr(1);
                            int i = 0;
                            for (auto& sv : split(s, "_")) {
                                sub[i] = numInterpretInt(sv);
                                i++;
                            }
                        }
                        break;
                    case '*':
                        if (true) {
                            string s = i.code.substr(1);
                            int i = 0;
                            for (auto& sv : split(s, "_")) {
                                mul[i] = numInterpretInt(sv);
                                i++;
                            }
                        }
                        break;
                    default:
                        codewconsts.code.push_back(i.code);
                }
            }
            print_debug("Finishing build stage 3 on callstack " + callstack.back());
        }
        void stage3tostage4asm() {
            AsmCode_Obj<unsigned short> new_com = codewconsts;
            codewconsts.code.clear();
            print_debug("Starting build stage 4 on callstack " + callstack.back());
            for (const CodeSegment<unsigned short>& i : new_com.code) {
                if (i.is_string) {
                    if (i.code.length() == 0) continue;
                    if (in_org) {
                        in_org = false;
                        try {
                            codewconsts.pos = numInterpretInt(i.code);

                        } catch (std::exception e) {
                            print_debug((string)"Failed to interpret number:" + e.what());
                            throw_err_comp("Error interpreting number: " + i.code);
                        }
                    }
                    else if (i.code == ".org") {
                        in_org = true;
                    } else if (i.code[0] == ':') {
                        if (isIn(labels,i.code)) {
                            throw_err("Cannot redefine label " + i.code);
                        } else {
                            labels[i.code.substr(1)] = codewconsts.pos;
                        }
                    } else {
                        print_debug("Found a reference to a possible label: " + i.code);
                        codewconsts.pos+=3;
                        codewlabels.code.push_back(i);
                    }
                } else {
                    codewlabels.code.push_back(i);
                    ++codewconsts.pos;
                }
            }
            print_debug("Finishing build stage 4 on callstack " + callstack.back());
        }
        void stage4tofinalasm() {
            AsmCode_Obj<unsigned short> new_com = codewlabels;
            codewlabels.code.clear();
            print_debug("Finishing build on callstack " + callstack.back());
            for (const CodeSegment<unsigned short>& i : new_com.code) {
                if (i.is_string) {
                    if (isIn(labels,i.code)) {
                        unsigned int pos = labels[i.code];
                        for (int i = 0; i < addrlen; i++) {
                            finishedcode.code.push_back(pos & 0xFF);
                            pos >>= 8;
                        }
                    } else throw_err_comp("Uninterpreted string found while finalizing \"" + i.code + "\"");
                } else {
                    finishedcode.code.push_back(i);
                }
            }
            print_debug("Finished build on callstack " + callstack.back());
        }
        vector <string> includedalready;
        vector <string> uncommentedcode; //stage 1 code
        AsmCode_Obj<unsigned short> codewdefs; //stage 2 code
        AsmCode_Obj<unsigned short> codewconsts; //stage 3 code
        AsmCode_Obj<unsigned short> codewlabels; //stage 4 code
        AsmCode_Obj<unsigned short> finishedcode; // final code
        unordered_map <string,string> indexedfilenames;
        char combine = 0;
        char add[2] = {0};
        char sub[2] = {0};
        char mul[2] = {0};
        bool doingop = false;
        //index all filenames in the program's directory and subdirectories
        void index_filenames() {
            if (indexedfilenames.size() == 0) {
                for (const auto& entry : fs::recursive_directory_iterator("./")) {
                    if (!entry.is_directory()) {
                        if (isIn(indexedfilenames,entry.path().filename().string())) continue;
                        indexedfilenames[entry.path().filename().string()] = entry.path().string();
                    }
                }
            }
        }
        void throw_err_comp(string message) {
            print_error(message + " --- " + callstack.back());
            print_debug("Callstack (from first call):");
            for (string s : callstack) {
                print_debug(s);
            }
            print_debug("End of callstack.");
            print_fatal_error("Cannot continue build process.");
            throw RuntimeError(message);
        }
        void throw_err(string message) {
            print_error(message + " --- " + callstack.back());
            print_debug("Callstack (from first call):");
            for (string s : callstack) {
                print_debug(s);
            }
            print_debug("End of callstack.");
            throw RuntimeError(message);
        }
        void include_filename(string filename) {
            index_filenames();
            if (!isIn(includedalready, filename)) {
                includedalready.push_back(filename);
                std::ifstream file (indexedfilenames[filename]);
                if (file.is_open()) {
                    string file_f;
                    while (true) {
                        char c = file.get();
                        if (file.eof()) break;
                        file_f += c;
                    }
                    file.close();
                    callstack.push_back(split(filename, '.')[0]);
                    print_debug("Including file: " + filename);
                    uncomment(file_f);
                    stage1tostage2asm();
                    print_debug("Finished including file: " + filename);
                    callstack.pop_back();
                } else {
                    throw_err_comp("Cannot open file for include " + filename);
                }
            }
        }
        void do_def(string def,string curr_def) {
            if (combine == 1) {
                CodeSegment<unsigned short> f = codewdefs.code.back();
                codewdefs.code.pop_back();
                combine = 0;
                if (f.is_string) do_def(f.code + def,curr_def);
                else do_def(std::to_string((int)f.value) + def,curr_def);
            } else if (combine == 2) {
                size_t s = codewdefs.code.size();
                CodeSegment<unsigned short> f = codewdefs.code.back();
                codewdefs.code.pop_back();
                combine = 0;
                do_def(def,curr_def);
                if (s >= codewdefs.code.size()) {
                    vector<CodeSegment<unsigned short>> new_code;
                    while ((codewdefs.code.size() + 1) > s) {
                        new_code.push_back(codewdefs.code.back());
                        codewdefs.code.pop_back();
                    }
                    CodeSegment<unsigned short> v = new_code.front();
                    new_code.erase(new_code.begin());
                    if (f.is_string) {
                        if (v.is_string) {
                            do_def(f.code + v.code,curr_def);
                        } else {
                            do_def(f.code + std::to_string((int)v.value),curr_def);
                        }
                    } else {
                        if (v.is_string) {
                            do_def(std::to_string((int)f.value) + v.code,curr_def);
                        } else {
                            do_def(std::to_string((int)f.value) + std::to_string((int)v.value),curr_def);
                        }
                    }
                    codewdefs.code.insert(codewdefs.code.end(),new_code.begin(),new_code.end());
                } else {
                    codewdefs.code.push_back(f);
                    combine = 2;
                }
            } else if (isIn(definitions,def)) {
                print_debug("Found definition call for " + def);
                if (def == curr_def) {
                    throw_err_comp("Cannot have a recursive definition call for " + def);
                }
                callstack.push_back(def);
                uncomment(definitions[def].def);
                stage1tostage2asm();
                callstack.pop_back();
                print_debug("Ended definition call for " + def);
            } else {
                codewdefs.code.push_back(def);
            }
        }
        enum {
            NONE,
            DEF, //handled
            IF, //unimplemented
            IFDEF, //handled
            IFNDEF, //handled
            UNDEF, //handled
            FILL, //handled
            FILLEMP, //handled
            INCLUDE, //handled
            ERROR //handled
        } inst;
        bool in_org = false;
        unsigned long long posix = 0;
        temp_inst_t temp_inst[3];
        unsigned long long exec = 0;
    };
    class Assembler<32> : protected OldAssembler<32> {
    public:
        using OldAssembler::addrlen;
        using OldAssembler::consts;
        using OldAssembler::verbose;
        vector <unsigned long> build(string asm_code) {
            try {
                clock = getTime();
                srand(time(NULL));
                callstack.push_back("__main__0x" + to_string(FULL_RAND));
                print_info("Started build for main");
                uncomment(asm_code);
                stage1tostage2asm();
                stage2tostage3asm();
                stage3tostage4asm();
                stage4tofinalasm();
                vector<unsigned long> ret;
                for (auto const& i : finishedcode.code) {
                    ret.insert(ret.end(),i.value);
                }
                print_info("Finished build for main");
                callstack.pop_back();
                return ret;
            } catch (std::exception e) {
                print_fatal_error(e.what());
                throw e;
            }
        };
        void reset() {
            uncommentedcode.clear();
            codewdefs.reset();
            codewconsts.reset();
            codewlabels.reset();
            definitions.clear();
            labels.clear();
            includedalready.clear();
            in_org = false;
            inst = NONE;
            posix = 0;
            exec = 0;
            add[0] = 0;
            add[1] = 0;
            sub[0] = 0;
            sub[1] = 0;
            mul[0] = 0;
            mul[1] = 0;
            doingop = false;
        }
        Assembler() {}
    protected:
        void uncomment(string asm_code) {
            string buffer;
            bool inComment = false;
            asm_code+= " ";
            print_debug("Beginning build stage 1 for callstack " + callstack.back());
            for (char c : asm_code) switch (c) {
                case ' ':
                case '\n':
                case '\t':
                    if (buffer.length() != 0) {
                        if (inComment && buffer == "*/") {
                            inComment = false;
                            buffer.clear();
                        } else if (!inComment && buffer == "/*") {
                            inComment = true;
                            buffer.clear();
                        } else if (!inComment) {
                            if (count(buffer, '"') == 0 
                            || (count(buffer,'"') > 0 && !startsWith(buffer,"0s")) 
                            || count(buffer,'"') % 2 == 0) {
                                uncommentedcode.push_back(buffer);
                                buffer.clear();
                            } else if (count(buffer,'"') % 2 == 1) {
                                buffer.push_back(c);
                            }
                        } else {
                            buffer.clear();
                        }
                    }
                    break;
                default:
                    buffer += c;
                    break;
            }
            print_debug("Finishing build stage 1 for callstack " + callstack.back());
        }
        void stage1tostage2asm() {
            vector<string> new_com = uncommentedcode;
            uncommentedcode.clear();
            print_debug("Beginning build stage 2 on callstack " + callstack.back());
            for (string s : new_com) {
                if (s == "__RANDOM_ID__" && posix == 1) {
                    std::stringstream st;
                    st << "0x" << std::hex << FULL_RAND;
                    s = st.str();
                }
                if (exec > 0) {
                    if (s == ".def") exec++;
                    else if (s == ".if") exec++;
                    else if (s == ".ifdef") exec++;
                    else if (s == ".ifndef") exec++;
                    else if (s == ".end") exec--;
                    else if (s == ".else" && exec == 1) exec--;
                }
                if (exec == 0 || (exec == 1 && (s == ".elif" || s == ".elifdef" || s == ".elifndef"))) {
                    bool insf = false;
                    if (s[0] == '.' && inst == NONE) {
                        if (s == ".emp") inst = FILLEMP;
                        else if (s == ".fill") inst = FILL;
                        else if (s == ".def") inst = DEF;
                        else if (s == ".if") inst = IF;
                        else if (s == ".undef") inst = UNDEF;
                        else if (s == ".ifdef") inst = IFDEF;
                        else if (s == ".elif") inst = IF;
                        else if (s == ".elifdef") inst = IFDEF;
                        else if (s == ".elifndef") inst = IFNDEF;
                        else if (s == ".error") inst = ERROR;
                        else if (s == ".include") inst = INCLUDE;
                        else insf = true;
                        if (!insf) posix = 0;
                    } else insf = true;
                    if (insf) switch(inst) {
                        case FILLEMP:
                            if (true) {
                                unsigned long long a = numInterpretInt(s);
                                for (unsigned int i = 0; i < a; i++) codewdefs.code.push_back(0);
                                print_debug("Filling " + to_string(a) + " bytes with 0.");
                                inst = NONE;
                                break;
                            }
                        case FILL:
                            if (posix == 0) {
                                temp_inst[0].i = numInterpretInt(s);
                                posix = 1;
                            } else {
                                unsigned long long a = numInterpretInt(s);
                                for (unsigned long long i = 0; i < a; i++) codewdefs.code.push_back(temp_inst[0].i);
                                print_debug("Filling " + to_string(a) + " bytes with " + to_string(temp_inst[0].i));
                                posix = 0;
                                inst = NONE;
                            }
                            break;
                        case DEF:
                            if (posix == 0) {
                                temp_inst[0].s = s;
                                temp_inst[1].s = string("");
                                posix = 1;
                            } else {
                                if (s == ".end") posix--;
                                else if (s == ".def") posix++;
                                else if (s == ".if") posix++;
                                else if (s == ".ifdef") posix++;
                                else if (s == ".ifndef") posix++;
                                if (posix == 0) {
                                    temp_inst[1].s.pop_back();
                                    definitions[temp_inst[0].s] = Definition(temp_inst[1].s);
                                    print_debug("Created definition for " + temp_inst[0].s);
                                    inst = NONE;
                                } else {
                                    temp_inst[1].s += s + " ";
                                }
                            }
                            break;
                        case UNDEF:
                            if (definitions.find(s) != definitions.end()) {
                                definitions.erase(s);
                                print_debug("Removed definition for " + s);
                            } else {
                                print_warning("Attempted removal of definition for " + s + " but it does not exist.");
                            }
                            inst = NONE;
                            break;
                        case IF:
                            if (posix == 0) {
                                temp_inst[0].s = s + " ";
                                posix++;
                            } else if (s == ".then") {
                                posix = 0;
                                inst = NONE;
                                temp_inst[0].s.pop_back();
                                print_warning("Use of .if but its not implemented in callstack " + callstack.back());
                                //unimplemented
                            } else temp_inst[0].s += s + " ";
                            break;
                        case IFDEF:
                            if (posix == 0) {
                                temp_inst[0].s = s + " ";
                                posix++;
                            } else if (s == ".then") {
                                posix = 0;
                                inst = NONE;
                                temp_inst[0].s.pop_back();
                                if (definitions.find(temp_inst[0].s) == definitions.end()) {
                                    exec++;
                                    print_debug(".ifdef evaluation for " + temp_inst[0].s + " is false.");
                                } else {
                                    print_debug(".ifdef evaluation for " + temp_inst[0].s + " is true.");
                                }
                            } else temp_inst[0].s += s + " ";
                            break;
                        case IFNDEF:
                            if (posix == 0) {
                                temp_inst[0].s = s + " ";
                                posix++;
                            } else if (s == ".then") {
                                posix = 0;
                                inst = NONE;
                                temp_inst[0].s.pop_back();
                                if (definitions.find(temp_inst[0].s) != definitions.end()) {
                                    exec++;
                                    print_debug(".ifndef evaluation for " + temp_inst[0].s + " is false.");
                                } else {
                                    print_debug(".ifndef evaluation for " + temp_inst[0].s + " is true.");
                                }
                            } else temp_inst[0].s += s + " ";
                            break;
                        case NONE:
                            if (s == "##") {
                                combine = 2;
                            } else if (s == "#"){
                                combine = 1;
                            } else do_def(s,callstack.back());
                            break;
                        case INCLUDE:
                            if (posix == 0) {
                                temp_inst[0].s = slice(s, 1);
                                if (count((temp_inst[0].s), '"') == 1) {
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-1);
                                    include_filename(temp_inst[0].s);
                                } else posix++;
                                
                            } else {
                                temp_inst[0].s += s + " ";
                                if (count((temp_inst[0].s), '"') == 1) {
                                    posix--;
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-2);
                                    include_filename(temp_inst[0].s);
                                }
                            }
                            break;
                        case ERROR:
                            if (posix == 0) {
                                temp_inst[0].s = slice(s, 1);
                                if (count((temp_inst[0].s), '"') == 2) {
                                    posix--;
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-1);
                                    throw_err_comp("Error directive: " + temp_inst[0].s);
                                }
                                posix++;
                            } else {
                                temp_inst[0].s += s + " ";
                                if (count((temp_inst[0].s), '"') == 2) {
                                    posix--;
                                    inst = NONE;
                                    temp_inst[0].s = slice(temp_inst[0].s, 0, temp_inst[0].s.length()-2);
                                    throw_err_comp("Error directive: " + temp_inst[0].s);
                                }
                            }
                            break;
                    }
                }
            }
            print_debug("Finishing build stage 2 on callstack " + callstack.back());
        }
        void stage2tostage3asm() {
            AsmCode_Obj<unsigned long> new_com = codewdefs;
            codewdefs.reset();
            print_debug("Starting build stage 3 on callstack " + callstack.back());
            for (const CodeSegment<unsigned long>& i : new_com.code) {
                if (!i.is_string) {
                    codewconsts.code.push_back(i);
                } else if (isIn(consts,i.code)) {
                    codewconsts.code.push_back(consts[i.code]);
                } else switch(i.code[0]) {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        if (doingop) {
                            if (add[0] > 0) {
                                for (auto& a : add) if (a > 4) {
                                    print_warning("Attempted to do an add operation of more than 4 bytes. 4 bytes will be used.");
                                    a = 4;
                                }
                                int v = max(add[0], add[1]);
                                unsigned int a = 0;
                                unsigned int b = numInterpretInt(i.code);
                                for (int i = 0; i < add[0]; i++) {
                                    a += codewconsts.code.back().value * pow(256, add[0]-i);
                                    codewconsts.code.pop_back();
                                }
                                vector<unsigned long> ve;
                                for (int i = 0; i < v; i++) {
                                    ve.push_back((a + b) >> (8*i));
                                }
                                codewconsts.code.insert(codewconsts.code.end(),ve.begin(),ve.end());
                                doingop = false;
                                add[0] = 0;
                                add[1] = 0;
                            } else if (sub[0] > 0) {
                                for (auto& a : sub) if (a > 4) {
                                    print_warning("Attempted to do a sub operation of more than 4 bytes. 4 bytes will be used.");
                                    a = 4;
                                }
                                int v = max(sub[0], sub[1]);
                                unsigned int a = 0;
                                unsigned int b = numInterpretInt(i.code);
                                for (int i = 0; i < sub[0]; i++) {
                                    a += codewconsts.code.back().value * pow(256, sub[0]-i);
                                    codewconsts.code.pop_back();
                                }
                                vector<unsigned long> ve;
                                for (int i = 0; i < v; i++) {
                                    ve.push_back((a - b) >> (8*i));
                                }
                                codewconsts.code.insert(codewconsts.code.end(),ve.begin(),ve.end());
                                doingop = false;
                                sub[0] = 0;
                                sub[1] = 0;
                            } else if (mul[0] > 0) {
                                for (auto& a : mul) if (a > 4) {
                                    print_warning("Attempted to do a mul operation of more than 4 bytes. 4 bytes will be used.");
                                    a = 4;
                                }
                                int v = max(mul[0], mul[1]);
                                unsigned int a = 0;
                                unsigned int b = numInterpretInt(i.code);
                                for (int i = 0; i < mul[0]; i++) {
                                    a += codewconsts.code.back().value * pow(256, mul[0]-i);
                                    codewconsts.code.pop_back();
                                }
                                vector<unsigned long> ve;
                                for (int i = 0; i < v; i++) {
                                    ve.push_back((a * b) >> (8*i));
                                }
                                codewconsts.code.insert(codewconsts.code.end(),ve.begin(),ve.end());
                            }
                        } else {
                            try {
                                vector<unsigned long> v = numInterpret(i.code);
                                codewconsts.code.insert(codewconsts.code.end(),v.begin(),v.end());
                            } catch (std::exception e) {
                                print_debug((string)"Failed to interpret number:" + e.what());
                                throw_err_comp("Error interpreting number: " + i.code);
                            }
                        }
                        break;
                    case '+':
                        if (true) {
                            string s = i.code.substr(1);
                            int i = 0;
                            for (auto& sv : split(s, "_")) {
                                add[i] = numInterpretInt(sv);
                                i++;
                            }
                        }
                        break;
                    case '-':
                        if (true) {
                            string s = i.code.substr(1);
                            int i = 0;
                            for (auto& sv : split(s, "_")) {
                                sub[i] = numInterpretInt(sv);
                                i++;
                            }
                        }
                        break;
                    case '*':
                        if (true) {
                            string s = i.code.substr(1);
                            int i = 0;
                            for (auto& sv : split(s, "_")) {
                                mul[i] = numInterpretInt(sv);
                                i++;
                            }
                        }
                        break;
                    default:
                        codewconsts.code.push_back(i.code);
                }
            }
            print_debug("Finishing build stage 3 on callstack " + callstack.back());
        }
        void stage3tostage4asm() {
            AsmCode_Obj<unsigned long> new_com = codewconsts;
            codewconsts.code.clear();
            print_debug("Starting build stage 4 on callstack " + callstack.back());
            for (const CodeSegment<unsigned long>& i : new_com.code) {
                if (i.is_string) {
                    if (i.code.length() == 0) continue;
                    if (in_org) {
                        in_org = false;
                        try {
                            codewconsts.pos = numInterpretInt(i.code);

                        } catch (std::exception e) {
                            print_debug((string)"Failed to interpret number:" + e.what());
                            throw_err_comp("Error interpreting number: " + i.code);
                        }
                    }
                    else if (i.code == ".org") {
                        in_org = true;
                    } else if (i.code[0] == ':') {
                        if (isIn(labels,i.code)) {
                            throw_err("Cannot redefine label " + i.code);
                        } else {
                            labels[i.code.substr(1)] = codewconsts.pos;
                        }
                    } else {
                        print_debug("Found a reference to a possible label: " + i.code);
                        codewconsts.pos+=3;
                        codewlabels.code.push_back(i);
                    }
                } else {
                    codewlabels.code.push_back(i);
                    ++codewconsts.pos;
                }
            }
            print_debug("Finishing build stage 4 on callstack " + callstack.back());
        }
        void stage4tofinalasm() {
            AsmCode_Obj<unsigned long> new_com = codewlabels;
            codewlabels.code.clear();
            print_debug("Finishing build on callstack " + callstack.back());
            for (const CodeSegment<unsigned long>& i : new_com.code) {
                if (i.is_string) {
                    if (isIn(labels,i.code)) {
                        unsigned int pos = labels[i.code];
                        for (int i = 0; i < addrlen; i++) {
                            finishedcode.code.push_back(pos & 0xFF);
                            pos >>= 8;
                        }
                    } else throw_err_comp("Uninterpreted string found while finalizing \"" + i.code + "\"");
                } else {
                    finishedcode.code.push_back(i);
                }
            }
            print_debug("Finished build on callstack " + callstack.back());
        }
        vector <string> includedalready;
        vector <string> uncommentedcode; //stage 1 code
        AsmCode_Obj<unsigned long> codewdefs; //stage 2 code
        AsmCode_Obj<unsigned long> codewconsts; //stage 3 code
        AsmCode_Obj<unsigned long> codewlabels; //stage 4 code
        AsmCode_Obj<unsigned long> finishedcode; // final code
        unordered_map <string,string> indexedfilenames;
        char combine = 0;
        char add[2] = {0};
        char sub[2] = {0};
        char mul[2] = {0};
        bool doingop = false;
        //index all filenames in the program's directory and subdirectories
        void index_filenames() {
            if (indexedfilenames.size() == 0) {
                for (const auto& entry : fs::recursive_directory_iterator("./")) {
                    if (!entry.is_directory()) {
                        if (isIn(indexedfilenames,entry.path().filename().string())) continue;
                        indexedfilenames[entry.path().filename().string()] = entry.path().string();
                    }
                }
            }
        }
        void throw_err_comp(string message) {
            print_error(message + " --- " + callstack.back());
            print_debug("Callstack (from first call):");
            for (string s : callstack) {
                print_debug(s);
            }
            print_debug("End of callstack.");
            print_fatal_error("Cannot continue build process.");
            throw RuntimeError(message);
        }
        void throw_err(string message) {
            print_error(message + " --- " + callstack.back());
            print_debug("Callstack (from first call):");
            for (string s : callstack) {
                print_debug(s);
            }
            print_debug("End of callstack.");
            throw RuntimeError(message);
        }
        void include_filename(string filename) {
            index_filenames();
            if (!isIn(includedalready, filename)) {
                includedalready.push_back(filename);
                std::ifstream file (indexedfilenames[filename]);
                if (file.is_open()) {
                    string file_f;
                    while (true) {
                        char c = file.get();
                        if (file.eof()) break;
                        file_f += c;
                    }
                    file.close();
                    callstack.push_back(split(filename, '.')[0]);
                    print_debug("Including file: " + filename);
                    uncomment(file_f);
                    stage1tostage2asm();
                    print_debug("Finished including file: " + filename);
                    callstack.pop_back();
                } else {
                    throw_err_comp("Cannot open file for include " + filename);
                }
            }
        }
        void do_def(string def,string curr_def) {
            if (combine == 1) {
                CodeSegment<unsigned long> f = codewdefs.code.back();
                codewdefs.code.pop_back();
                combine = 0;
                if (f.is_string) do_def(f.code + def,curr_def);
                else do_def(std::to_string((int)f.value) + def,curr_def);
            } else if (combine == 2) {
                size_t s = codewdefs.code.size();
                CodeSegment<unsigned long> f = codewdefs.code.back();
                codewdefs.code.pop_back();
                combine = 0;
                do_def(def,curr_def);
                if (s >= codewdefs.code.size()) {
                    vector<CodeSegment<unsigned long>> new_code;
                    while ((codewdefs.code.size() + 1) > s) {
                        new_code.push_back(codewdefs.code.back());
                        codewdefs.code.pop_back();
                    }
                    CodeSegment<unsigned long> v = new_code.front();
                    new_code.erase(new_code.begin());
                    if (f.is_string) {
                        if (v.is_string) {
                            do_def(f.code + v.code,curr_def);
                        } else {
                            do_def(f.code + std::to_string((int)v.value),curr_def);
                        }
                    } else {
                        if (v.is_string) {
                            do_def(std::to_string((int)f.value) + v.code,curr_def);
                        } else {
                            do_def(std::to_string((int)f.value) + std::to_string((int)v.value),curr_def);
                        }
                    }
                    codewdefs.code.insert(codewdefs.code.end(),new_code.begin(),new_code.end());
                } else {
                    codewdefs.code.push_back(f);
                    combine = 2;
                }
            } else if (isIn(definitions,def)) {
                print_debug("Found definition call for " + def);
                if (def == curr_def) {
                    throw_err_comp("Cannot have a recursive definition call for " + def);
                }
                callstack.push_back(def);
                uncomment(definitions[def].def);
                stage1tostage2asm();
                callstack.pop_back();
                print_debug("Ended definition call for " + def);
            } else {
                codewdefs.code.push_back(def);
            }
        }
        enum {
            NONE,
            DEF, //handled
            IF, //unimplemented
            IFDEF, //handled
            IFNDEF, //handled
            UNDEF, //handled
            FILL, //handled
            FILLEMP, //handled
            INCLUDE, //handled
            ERROR //handled
        } inst;
        bool in_org = false;
        unsigned long long posix = 0;
        temp_inst_t temp_inst[3];
        unsigned long long exec = 0;
    };
    
};
using __assembler_namespace::Assembler;
using __assembler_namespace::OldAssembler;