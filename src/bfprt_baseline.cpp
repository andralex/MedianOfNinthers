/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#include "common.h"
#include <algorithm>

template <class T>
static T* partition(T* r, T* end)
{
    const size_t len = end - r;
    if (len < 5) return pivotPartition(r, len / 2, len);
    size_t j = 0;
    for (size_t i = 4; i < len; i += 5, ++j)
    {
        partition5(r, i - 4, i - 3, i, i - 2, i - 1);
        cswap(r[i], r[j]);
    }
    quickselect<double, &partition>(r, r + j / 2, r + j);
    return pivotPartition(r, j / 2, len);
}

void (*computeSelection)(double*, double*, double*)
    = &quickselect<double, &partition<double>>;
