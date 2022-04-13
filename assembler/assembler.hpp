#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <exception>

#include "stringUtilities.hpp"
#include "colors.hpp"

#pragma once

namespace __assembler_namespace {
    using namespace std;
    using namespace std::chrono;
    long long getTime() {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
    class Assembler;
    struct Definition {
        string def;
    };
    class Assembler {
    public:
        unsigned char addrlen = 2;
        std::unordered_map<std::string,unsigned char> consts;
        void start_build(string file) {
            print_info("Build started.");
            print_debug("Callstack main");
            callstack.push_back("main");
            clock = getTime();
            assemble(file);
        }
        void assemble(string file) {
            string buffer;
            for (unsigned char a : file) {
                switch(a) {
                    case '\r':
                    case '\n':
                    case ' ':
                        if (inComment) {
                            if (isIn(buffer, "*/")) {
                                inComment = false;
                                buffer = split(buffer, "*/")[1];
                            }
                        }
                        if (buffer.length() != 0) break;
                        switch (inst) {
                            case 0:
                                break;
                            case 'o':
                                pos = numInterpretInt(buffer);
                                pos--;
                                buffer.clear();
                                inst = 0;
                                break;
                            case 'e':
                                if (true) {
                                    unsigned int idc = numInterpretInt(buffer);
                                    vector<unsigned char> f(idc,0);
                                    exp.insert(exp.end(), f.begin(), f.end());
                                    pos += idc - 1;
                                    buffer.clear();
                                    inst = 0;
                                    break;
                                }
                            case 'f':
                                if (posix == 0) {
                                    posF = numInterpretInt(buffer);
                                    buffer.clear();
                                    posix++;
                                    pos--;
                                    break;
                                } else {
                                    vector<unsigned char> f(posF,numInterpretInt(buffer));
                                    pos += posF - 1;
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
                                    } else if (buffer == buffer_def) {
                                        print_fatal_error("Definition name is the same as the instruction name.");
                                        print_info("Macro name: " + buffer);
                                        print_debug("Printing callstack.");
                                        for (string s : callstack) {
                                            print_debug(s);
                                        }
                                        throw exception("Definition name is the same as the instruction name.");
                                    } else {
                                        buffer__def += buffer + " ";
                                        buffer.clear();
                                    }
                                }
                            case 'u':
                                definitions.erase(buffer);
                                buffer.clear();
                                inst = 0;
                                break;
                        }
                        if (buffer.length() != 0) break;
                        switch(buffer[0]) {
                                case 0:
                                case 1:
                                case 2:
                                case 3:
                                case 4:
                                case 5:
                                case 6:
                                case 7:
                                case 8:
                                case 9:
                                    if (true) {
                                        vector<unsigned char> v = numInterpret(buffer);
                                        exp.insert(exp.end(),v.begin(),v.end());
                                    }
                                case ':':
                                    labels[buffer.substr(1)] = pos;
                                    pos--;
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
                                    } else if (labels.find(buffer) != labels.end()) {
                                        exp.push_back(labels[buffer]);
                                    } else if (definitions.find(buffer) != definitions.end()) {
                                        callstack.push_back(buffer);
                                        print_debug("Callstack " + buffer);
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
                pos++;
            }
        }
        vector <unsigned char> finalize() {
            if (inst != 0) {
                print_fatal_error("unfinished preprocessor instruction");
                print_debug("Printing callstack");
                for (string s : callstack) {
                    print_debug(s);
                }
                throw exception("unfinished preprocessor instruction");
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
    protected:
        vector <unsigned char> numInterpret(string buffer) {
            vector <unsigned char> ret;
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) {
                        bool gud = true;
                        switch (buffer[1]) {
                            case 'x':
                                ret.push_back(stoi(buffer.substr(2), nullptr, 16));
                                break;
                            case 'o':
                                ret.push_back(stoi(buffer.substr(2), nullptr, 8));
                                break;
                            case 'b':
                                ret.push_back(stoi(buffer.substr(2), nullptr, 2));
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
                    ret.push_back(stoi(buffer, nullptr, 10));
                    break;
            }
            return ret;
        }
        unsigned long long numInterpretInt(string buffer) {
            switch (buffer[0]) {
                case '0':
                    if (buffer.length() > 2) switch (buffer[1]) {
                            case 'x':
                                return stoi(buffer.substr(2), nullptr, 16);
                            case 'o':
                                return stoi(buffer.substr(2), nullptr, 8);
                            case 'b':
                                return stoi(buffer.substr(2), nullptr, 2);
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
                    return stoi(buffer, nullptr, 10);
            }
            return 0;
        }
        unordered_map<string,unsigned int> labels;
        unordered_map<string,Definition> definitions;
        vector <unsigned char> exp;
        unsigned int pos = 0;
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
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_GREEN << round(getTime() - clock) / 1000 << " DEBUG " << info << COLOR_RESET << endl;
        }
        void print_fatal_error(string info) {
            cout.setf(std::ios::fixed,std::ios::floatfield);
            cout.precision(3);
            cout << COLOR_WHITE COLOR_BACKGROUND_RED << round(getTime() - clock) / 1000 << " FATAL " << info << COLOR_RESET << endl;
        }
    private:
        bool inComment = false;
        unsigned char inst = 0;
        unsigned int posix = 0;
        unsigned int posF = 0;
        string buffer_def;
        string buffer__def;
    };
};
using __assembler_namespace::Assembler;