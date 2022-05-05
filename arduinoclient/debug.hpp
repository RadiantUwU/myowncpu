#include <Arduino.h>
#include "serial.hpp"
#ifndef NDEBUG
void printDebug(const char* f) {
    SerialSend(sizeof(f) / sizeof(char),SerialInsts::Debug,(char*)f);
}
void printDebug(char* f, size_t n) {
    SerialSend(n,SerialInsts::Debug,f);
}
#else
void printDebug(const char* f) {}
void printDebug(char* f, size_t n) {}
#endif
const PROGMEM char hexv[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
void hexPrint(char* p, unsigned long long n, size_t b) {
    for (int i = b; i >= 0; i--) {
        p[i] = hexv[(n >> ((b - i) * 4)) % 16];
    }
}
