/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#include "common.h"
#include <random>

template <class T>
static T* partition(T* r, T* end)
{
    const size_t len = end - r;
    assert(len >= 3);
    static std::mt19937 gen(1);
    std::uniform_int_distribution<> dis(0, len - 1);
    size_t x = dis(gen), y = dis(gen), z = dis(gen);
    return pivotPartition(r, medianIndex(r, x, y, z), len);
}

void (*computeSelection)(double*, double*, double*)
    = &quickselect<double, &partition<double>>;
