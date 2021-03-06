#pragma once
#include "sf_string.h"

namespace skyfire
{

template <typename StringType>
basic_string<StringType>::basic_string(const StringType& str)
    : StringType(str)
{
}

template <typename StringType>
basic_string<StringType> basic_string<StringType>::repeat(const basic_string<StringType>& src, int count)
{
    auto                     len = src.length();
    basic_string<StringType> ret(len * count, '\0');

    for (int i = 0; i < count; ++i)
    {
        memcpy(ret.data() + i * len, src.data(), len);
    }
    return ret;
}

template <typename StringType>
basic_string<StringType> operator*(const basic_string<StringType>& str, int n)
{
    return basic_string<StringType>::repeat(str, n);
}
}