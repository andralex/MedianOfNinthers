/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#include "common.h"

template <class T>
static T* partition(T* r, T* end)
{
    const size_t n = end - r;
    size_t p;
    if (n >= 10)
    {
        auto k = n / 10;
        p = 5 * k;
        ninther(r, k, 2 * k, 3 * k, 4 * k, p, 6 * k, 7 * k, 8 * k, 9 * k);
    }
    else
    {
        p = n / 2;
    }
    return pivotPartition(r, p, n);
}

void (*computeSelection)(double*, double*, double*)
    = &quickselect<double, &partition<double>>;
