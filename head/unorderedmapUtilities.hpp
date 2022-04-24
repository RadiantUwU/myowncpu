#pragma once
#include <unordered_map>
using std::unordered_map;
//check if key is present in unordered_map
template <typename T, typename U>
bool isIn(unordered_map<T, U> map, T key)
{
    for (auto it = map.begin(); it != map.end(); it++)
    {
        if (it->first == key)
        {
            return true;
        }
    }
    return false;
}
//check if value is present in unordered_map
template <typename T, typename U>
bool isInV(unordered_map<T, U> map, U value)
{
    for (auto it = map.begin(); it != map.end(); it++)
    {
        if (it->second == value)
        {
            return true;
        }
    }
    return false;
}