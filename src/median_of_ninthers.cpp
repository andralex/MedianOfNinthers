/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#include "common.h"
#include <algorithm>

template <class T>
static size_t partitionImpl(T* beg, size_t length);
template <class T>
static void adaptiveQuickselect(T* beg, size_t n, size_t length);

/**
Median of minima for \Gamma = 2
*/
template <class T>
static size_t medianOfMinima2(T*const r, const size_t n, const size_t length)
{
    assert(length >= 2);
    const auto _2 = length / 2;
    assert(n < _2);
    for (size_t i = 0; i < _2; ++i)
    {
        if (r[i + _2] <CNT r[i]) cswap(r[i], r[i + _2]);
    }
    adaptiveQuickselect(r, n, _2);
    return expandPartition(r, 0, n, _2, length);
}

/**
Median of maxima for \Gamma = 2
*/
template <class T>
static size_t medianOfMaxima2(T*const r, const size_t n, const size_t length)
{
    assert(length >= 2);
    const auto _2 = length / 2, lo = length - _2;
    assert(n >= lo);
    for (size_t i = lo; i < length; ++i)
    {
        if (r[i - _2] >CNT r[i]) cswap(r[i - _2], r[i]);
    }
    adaptiveQuickselect(r + lo, n - lo, _2);
    return expandPartition(r, lo, n, length, length);
}

/**
Median of minima for \Gamma = 8
*/
template <class T>
static size_t medianOfMinima8(T*const r, const size_t n, const size_t length)
{
    assert(length >= 8);
    const auto _8 = length / 8;
    assert(n < _8);
    for (size_t i = 0, j = _8; i < _8; ++i, j += 7)
    {
        auto a = r[i] <=CNT r[j] ? i : j;
        auto b = r[j + 1] <=CNT r[j + 2] ? j + 1 : j + 2;
        auto c = r[j + 3] <=CNT r[j + 4] ? j + 3 : j + 4;
        auto d = r[j + 5] <=CNT r[j + 6] ? j + 5 : j + 6;
        auto e = r[b] <CNT r[a] ? b : a;
        auto f = r[d] <CNT r[c] ? d : c;
        auto g = r[f] <CNT r[e] ? f : e;
        cswap(r[i], r[g]);
    }
    adaptiveQuickselect(r, n, _8);
    return expandPartition(r, 0, n, _8, length);
}

/**
Median of maxima for \Gamma = 8
*/
template <class T>
static size_t medianOfMaxima8(T*const r, const size_t n, const size_t length)
{
    assert(length >= 8);
    const auto _8 = length / 8;
    const auto lo = length - _8;
    for (size_t i = lo, j = 0; i < length; ++i, j += 7)
    {
        auto a = r[j] >CNT r[j + 1] ? j : j + 1;
        auto b = r[j + 2] >CNT r[j + 3] ? j + 2 : j + 3;
        auto c = r[j + 4] >CNT r[j + 5] ? j + 4 : j + 5;
        auto d = r[j + 6] >CNT r[i] ? j + 6 : i;
        auto e = r[a] >CNT r[b] ? a : b;
        auto f = r[c] >CNT r[d] ? c : d;
        auto g = r[e] >CNT r[f] ? e : f;
        cswap(r[g], r[i]);
    }
    adaptiveQuickselect(r + lo, n - lo, _8);
    return expandPartition(r, lo, n, length, length);
}

/**
Partitions r[0 .. length] using a pivot of its own choosing. Attempts to pick a
pivot that approximates the median. Returns the position of the pivot.
*/
template <class T>
static size_t medianOfNinthers(T*const r, const size_t length)
{
    assert(length >= 12);
    const auto frac =
        length <= 1024 ? length / 12 :
        length <= 128 * 1024 ? length / 64
        : length / 1024;
    auto pivot = frac / 2;
    const auto lo = length / 2 - pivot, hi = lo + frac;
    assert(lo >= frac * 4);
    assert(length - hi >= frac * 4);
    assert(lo / 2 >= pivot);
    const auto gap = (length - 9 * frac) / 4;
    auto a = lo - 4 * frac - gap, b = hi + gap;
    for (size_t i = lo; i < hi; ++i, a += 3, b += 3)
    {
        ninther(r, a, i - frac, b, a + 1, i, b + 1, a + 2, i + frac, b + 2);
    }

    adaptiveQuickselect(r + lo, pivot, frac);
    return expandPartition(r, lo, lo + pivot, hi, length);
}

/**

Quickselect driver for medianOfNinthers, medianOfMinima, and medianOfMaxima.
Dispathes to each depending on the relationship between n (the sought order
statistics) and length.

*/
template <class T>
static void adaptiveQuickselect(T* r, size_t n, size_t length)
{
    assert(n < length);
    for (;;)
    {
        // Decide strategy for partitioning
        if (n == 0)
        {
            // That would be the max
            auto pivot = n;
            for (++n; n < length; ++n)
                if (r[n] <CNT r[pivot]) pivot = n;
            cswap(r[0], r[pivot]);
            return;
        }
        if (n + 1 == length)
        {
            // That would be the min
            auto pivot = 0;
            for (n = 1; n < length; ++n)
                if (r[pivot] <CNT r[n]) pivot = n;
            cswap(r[pivot], r[length - 1]);
            return;
        }
        assert(n < length);
        size_t pivot;
        if (length <= 16)
            pivot = pivotPartition(r, n, length) - r;
        else if (n * 16 <= length)
            pivot = medianOfMinima8(r, n, length);
        else if (n * 16 >= length * 15)
            pivot = medianOfMaxima8(r, n, length);
        else if (n * 4 <= length)
            pivot = medianOfMinima2(r, n, length);
        else if (n * 4 >= length * 3)
            pivot = medianOfMaxima2(r, n, length);
        else
            pivot = medianOfNinthers(r, length);

        // See how the pivot fares
        if (pivot == n)
        {
            return;
        }
        if (pivot > n)
        {
            length = pivot;
        }
        else
        {
            ++pivot;
            r += pivot;
            length -= pivot;
            n -= pivot;
        }
    }
}

template <class T>
static void quickselect(T* beg, T* mid, T* end)
{
    if (beg == end || mid >= end) return;
    assert(beg <= mid && mid < end);
    adaptiveQuickselect(beg, mid - beg, end - beg);
}

void (*computeSelection)(double*, double*, double*)
    = &quickselect<double>;
