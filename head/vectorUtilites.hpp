#include <vector>
#pragma once
using std::vector;
//check if value is present in vector
template <typename T>
bool isIn(vector<T> vec, T value)
{
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] == value)
        {
            return true;
        }
    }
    return false;
}
//check if value is present in vector
template <typename T>
bool isIn(vector<T> vec, T value, int &index)
{
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] == value)
        {
            index = i;
            return true;
        }
    }
    return false;
}
//check if value is present in vector
template <typename T>
bool isIn(vector<T> vec, T value, int &index, int &count)
{
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] == value)
        {
            index = i;
            count++;
        }
    }
    return false;
}