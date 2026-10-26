#pragma once

// 实验五：同一函数模板用于坐标(double)和能量(int)的范围限制。
template <typename T> T limitValue(T value, T lower, T upper)
{
    if (value < lower)
        return lower;
    if (value > upper)
        return upper;
    return value;
}
