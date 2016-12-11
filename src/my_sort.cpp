/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#include <algorithm>
#include "common.h"
#include "median_of_ninthers.h"

template <class T>
static size_t partition(T*const r, const size_t length)
{
    // adaptiveQuickselect(r, length / 2, length);
    // return length / 2;
    assert(length > 10);
    const size_t sample = length / 8;
    const size_t pivot = sample / 2;
    const auto lo = length / 2 - pivot, hi = lo + sample;
    adaptiveQuickselect(r + lo, pivot, sample);
    return expandPartition(r, lo, lo + pivot, hi, length);
}

template <class T, size_t (*partition)(T*, size_t)>
void quicksort(T* r, T* end)
{
    for (;;)
    {
        const size_t length = end - r;
        assert(length >= 5);

        if (length < 5000000)
        {
            return std::sort(r, end);
        }

        //auto pivot = partition(r, length);
        //const auto pivot = medianOfMinima3(r, 8 * length / 60, length);
        //const auto pivot = medianOfMinima4(r, 4 * length / 80, length);
        //const auto pivot = medianOfMinima8(r, 10 * length / 160, length);
        const auto sampleLen = length / 6;
        const auto pivot = medianOfMinima8(
            r, 10 * sampleLen / 160, sampleLen, length);

        if (pivot <= length / 2)
        {
            quicksort<T, partition>(r, r + pivot);
            r += pivot + 1;
            // if (pivot <= length / 8)
            // {
            //     const auto length2 = end - r;
            //     const auto pivot2 = medianOfMaxima8(
            //         r, 15 * length2 / 16, length2);
            //     quicksort<T, partition>(r + pivot2, end);
            //     end = r + pivot2;
            // }
        }
        else
        {
            quicksort<T, partition>(r + pivot + 1, end);
            end = r + pivot;
        }
    }
}

void (*compute)(double*, double*)
    = &quicksort<double, partition>;
