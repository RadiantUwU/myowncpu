#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <chrono>

#include "stringUtilities.hpp"

namespace __assembler_namespace {
    using namespace std;
    using namespace std::chrono;
    long long getTime() {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
    class Assembler;
    class Assembler {
    public:
        std::unordered_map<std::string,unsigned char> consts;
        void assemble(string file) {
            string buffer;
            for (unsigned char a : file) {
                switch(a) {
                    case '\r':
                    case '\n':
                    case ' ':
                        if (buffer.length() != 0) {
                            switch(buffer[0]) {
                                case '0':
                                    if (buffer.length() > 2) {
                                        bool gud = true;
                                        switch (buffer[1]) {
                                            case 'x':
                                                exp.push_back(stoi(buffer.substr(2), nullptr, 16));
                                                break;
                                            case 'o':
                                                exp.push_back(stoi(buffer.substr(2), nullptr, 8));
                                                break;
                                            case 'b':
                                                exp.push_back(stoi(buffer.substr(2), nullptr, 2));
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
                                    exp.push_back(stoi(buffer, nullptr, 10));
                                    break;
                                case ':':
                                    labels[buffer.substr(1)] = pos;
                                    pos--;
                                    break;
                            }
                        }
                        break;
                    default:
                        buffer += a;
                        break;
                }
                pos++;
            }
        }
        vector <unsigned char> finalize() {
            vector<unsigned char> ret = exp;
            exp.clear();
            labels.clear();
            pos = 0;
            return ret;
        }
    protected:
        std::unordered_map<std::string,unsigned int> labels;
        vector <unsigned char> exp;
        unsigned int pos = 0;
    };
};