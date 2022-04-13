#include <Arduino.h>
void memcopy(void* src, void* dest, unsigned long long size) {
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
#include "./config.cpp"
#include "./serial.cpp"
#include "./emulator.cpp"