/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#include "common.h"
#include <algorithm>

template <class T>
size_t partitionImpl(T* beg, size_t length);
template <class T>
void adaptiveQuickselect(T* beg, size_t n, size_t length);

/**
Median of minima for \Gamma = 2
*/
template <class T>
size_t medianOfMinima2(T*const r, const size_t n, const size_t length)
{
    assert(length >= 2);
    const auto _2 = length / 2;
    assert(n < _2);
    for (size_t i = 0; i < _2; ++i)
    {
        if (r[i + _2] < r[i]) std::swap(r[i], r[i + _2]);
    }
    adaptiveQuickselect(r, n, _2);
    return expandPartition(r, 0, n, _2, length);
}

/**
Median of maxima for \Gamma = 2
*/
template <class T>
size_t medianOfMaxima2(T*const r, const size_t n, const size_t length)
{
    assert(length >= 2);
    const auto _2 = length / 2, lo = length - _2;
    assert(n >= lo);
    for (size_t i = lo; i < length; ++i)
    {
        if (r[i - _2] > r[i]) std::swap(r[i - _2], r[i]);
    }
    adaptiveQuickselect(r + lo, n - lo, _2);
    return expandPartition(r, lo, n, length, length);
}

/**
Median of minima for \Gamma = 4
*/
template <class T>
size_t medianOfMinima4(T*const r, const size_t n, const size_t length)
{
    assert(length >= 4);
    const auto _4 = length / 4;
    assert(n < _4);
    for (size_t i = 0, j = _4; i < _4; ++i, j += 3)
    {
        auto a = r[j] <= r[j + 1] ? j : j + 1;
        if (r[a] > r[j + 2]) a = j + 2;
        if (r[a] < r[i]) std::swap(r[i], r[a]);
    }
    adaptiveQuickselect(r, n, _4);
    return expandPartition(r, 0, n, _4, length);
}

/**
Median of minima for \Gamma = 4
*/
template <class T>
size_t medianOfMinima3(T*const r, const size_t n, const size_t length)
{
    assert(length >= 3);
    const auto _3 = length / 3;
    assert(n < _3);
    for (size_t i = 0; i < _3; ++i)
    {
        const auto j = _3 + i * 2;
        const auto a = r[j] <= r[j + 1] ? j : j + 1;
        if (r[a] < r[i]) std::swap(r[i], r[a]);
    }
    adaptiveQuickselect(r, n, _3);
    return expandPartition(r, 0, n, _3, length);
}

/**
Median of minima for \Gamma = 8
*/
template <class T>
size_t medianOfMinima8(T*const r, const size_t n, const size_t length)
{
    assert(length >= 8);
    const auto _8 = length / 8;
    assert(n < _8);
    for (size_t i = 0, j = _8; i < _8; ++i, j += 7)
    {
        auto a = r[i] <= r[j] ? i : j;
        auto b = r[j + 1] <= r[j + 2] ? j + 1 : j + 2;
        auto c = r[j + 3] <= r[j + 4] ? j + 3 : j + 4;
        auto d = r[j + 5] <= r[j + 6] ? j + 5 : j + 6;
        auto e = r[b] < r[a] ? b : a;
        auto f = r[d] < r[c] ? d : c;
        auto g = r[f] < r[e] ? f : e;
        std::swap(r[i], r[g]);
    }
    adaptiveQuickselect(r, n, _8);
    return expandPartition(r, 0, n, _8, length);
}

/**
Median of maxima for \Gamma = 8
*/
template <class T>
size_t medianOfMaxima8(T*const r, const size_t n, const size_t length)
{
    assert(length >= 8);
    const auto _8 = length / 8, rem = length % 8, limit = _8 + rem;
    const auto lo = length - _8;
    for (size_t i = lo, j = rem; i < length; ++i, j += 7)
    {
        auto a = r[j] > r[j + 1] ? j : j + 1;
        auto b = r[j + 2] > r[j + 3] ? j + 2 : j + 3;
        auto c = r[j + 4] > r[j + 5] ? j + 4 : j + 5;
        auto d = r[j + 6] > r[i] ? j + 6 : i;
        auto e = r[a] > r[b] ? a : b;
        auto f = r[c] > r[d] ? c : d;
        auto g = r[e] > r[f] ? e : f;
        std::swap(r[g], r[i]);
    }
    adaptiveQuickselect(r + lo, n - lo, _8);
    return expandPartition(r, lo, n, length, length);
}

/**
Partitions r[0 .. length] using a pivot of its own choosing. Attempts to pick a
pivot that approximates the median. Returns the position of the pivot.
*/
template <class T>
size_t medianOfNinthers(T*const r, const size_t length)
{
    assert(length >= 9);
    // _9 is one ninth of the entire sample size
    auto _9 = length < 1000 ? length / 10 : length / 300;
    if (_9 == 0)
    {
        return nintherPartition(r, r + length) - r;
    }
    auto pivot = _9 / 2;
    const auto lo = length / 2 - pivot, hi = lo + _9;
    assert(length >= 9 * _9);
    const auto gap = (length - 9 * _9) / 4;

    auto a = lo - 4 * _9 - gap, b = hi + _9 + gap;
    assert(a + 3 * _9 <= lo - _9);
    assert(hi + _9 <= b);
    for (size_t i = lo; i < hi; ++i, a += 3, b += 3)
    {
        ninther(r, a, a + 1, a + 2, i - _9, i, i + _9, b, b + 1, b + 2);
    }

    adaptiveQuickselect(r + lo, pivot, _9);
    return expandPartition(r, lo, lo + pivot, hi, length);
}

template <class T>
size_t medianOfFifteenthers(T*const r, const size_t length)
{
    // _15 is one fifteenth of the entire sample size
    auto _15 = length / 600;
    if (_15 == 0)
    {
        return nintherPartition(r, r + length) - r;
    }
    auto pivot = _15 / 2;
    const auto lo = length / 2 - pivot, hi = lo + _15;
    assert(length >= 15 * _15);
    const auto gap = (length - 15 * _15) / 4;

    assert(lo >= 7 * _15 + gap);
    auto a = lo - 7 * _15 - gap, b = hi + 2 * _15 + gap;
    for (size_t i = lo; i < hi; ++i, a += 5, b += 5)
    {
        fifteenther(r, a, a + 1, a + 2, a + 3, a + 4, i - 2 * _15, i - _15,
            i, i + _15, i + 2 * _15, b, b + 1, b + 2, b + 3, b + 4);
    }

    adaptiveQuickselect(r + lo, pivot, _15);
    return expandPartition(r, lo, lo + pivot, hi, length);
}

template <class T>
size_t medianOf27s(T*const r, const size_t length)
{
    const auto q/*uantum*/ = (length / 27) / 20;
    if (q == 0 || q * 27 > length)
    {
        return nintherPartition(r, r + length) - r;
    }
    const auto halfQ = q / 2;
    const auto chunkSize = q * 9; // 3 chunks
    //printf("median27 with sample %zu of %zu\n", chunkSize * 3, length);
    assert(length >= 3 * chunkSize);
    const auto gap = (length - 3 * chunkSize) / 4;

    const size_t lo = length / 2 - halfQ;
    const size_t hi = lo + q;
    size_t midLeft = lo - 4 * q, midRite = hi;
    size_t left = midLeft - gap - chunkSize, rite = midRite + gap;
    const size_t left2rite = 2 * (chunkSize + gap);
    const size_t midLeft2midRite = 5 * q;

    for (size_t i = lo; i < hi; left += 9, midLeft += 4, ++i)
    {
        const auto m1 = nintherIndex(r, left, left + 1, left + 2, left + 3,
            left + 4, left + 5, left + 6, left + 7, left + 8);
        const auto midRite = midLeft + midLeft2midRite;
        const auto m2 = nintherIndex(r,
            midLeft, midLeft + 1, midLeft + 2, midLeft + 3, i,
            midRite, midRite + 1, midRite + 2, midRite + 3);
        const auto rite = left + left2rite;
        const auto m3 = nintherIndex(r, rite, rite + 1, rite + 2, rite + 3,
            rite + 4, rite + 5, rite + 6, rite + 7, rite + 8);
        std::swap(r[i], r[median3Index(r, m1, m2, m3)]);
    }

    adaptiveQuickselect(r + lo, halfQ, q);
    return expandPartition(r, lo, lo + halfQ, hi, length);
}

/**

Quickselect driver for medianOfNinthers, medianOfMinima, and medianOfMaxima.
Dispathes to each depending on the relationship between n (the sought order
statistics) and length.

*/
template <class T>
void adaptiveQuickselect(T* r, size_t n, size_t length)
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
                if (r[n] < r[pivot]) pivot = n;
            std::swap(r[0], r[pivot]);
            return;
        }
        if (n + 1 == length)
        {
            // That would be the min
            auto pivot = 0;
            for (n = 1; n < length; ++n)
                if (r[pivot] < r[n]) pivot = n;
            std::swap(r[pivot], r[length - 1]);
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
