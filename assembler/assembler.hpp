#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <exception>

#include "stringUtilities.hpp"

#pragma once

namespace __assembler_namespace {
    using namespace std;
    using namespace std::chrono;
    long long getTime() {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
    class Assembler;
    struct Definition {
        bool recursive = false;
        string label;
    };
    class Assembler {
    public:
        unsigned char addrlen = 2;
        std::unordered_map<std::string,unsigned char> consts;
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
                        switch (inst) {
                            case 0:
                                break;
                            case 'o':
                                pos = 
                        }
                        if (buffer.length() != 0) {
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
                                    else if (buffer == "recursivedef") inst = 'r';
                                    break;
                                case '/':
                                    if (buffer == "/*") {
                                        inComment = true;
                                    }
                                default:

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
            if (inst != 0)
            vector<unsigned char> ret = exp;
            exp.clear();
            labels.clear();
            pos = 0;
            return ret;
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
        unordered_map<string,unsigned int> labels;
        unordered_map<string,Definition> definitions;
        vector <unsigned char> exp;
        unsigned int pos = 0;
    private:
        bool inComment = false;
        unsigned char inst = 0;
        unsigned char posix = 0;
    };
};
using __assembler_namespace::Assembler;