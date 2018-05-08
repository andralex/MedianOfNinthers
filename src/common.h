/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#pragma once
#include <algorithm>
#include <cassert>

/**
Instrumentation counters
*/
#ifdef COUNT_SWAPS
extern unsigned long g_swaps;
#endif
#ifdef COUNT_WASTED_SWAPS
extern unsigned long g_wastedSwaps;
#endif
#ifdef COUNT_COMPARISONS
extern unsigned long g_comparisons;
#endif

/**
Instrumented swap
*/
template <class T>
inline void cswap(T& lhs, T& rhs)
{
    std::swap(lhs, rhs);
#ifdef COUNT_SWAPS
    ++g_swaps;
#endif
#ifdef COUNT_WASTED_SWAPS
    if (lhs == rhs) ++g_wastedSwaps;
#endif
}

/**
Instrumented comparisons
*/
#ifdef COUNT_COMPARISONS
#define CNT (++g_comparisons, 0)+
#else
#define CNT
#endif

/**
Swaps the median of r[a], r[b], and r[c] into r[b].
*/
template <class T>
void median3(T* r, size_t a, size_t b, size_t c)
{
    if (r[b] <CNT r[a]) // b < a
    {
        if (r[b] <CNT r[c]) // b < a, b < c
        {
            if (r[c] <CNT r[a]) // b < c < a
                cswap(r[b], r[c]);
            else  // b < a <= c
                cswap(r[b], r[a]);
        }
    }
    else if (r[c] <CNT r[b]) // a <= b, c < b
    {
        if (r[c] <CNT r[a]) // c < a <= b
            cswap(r[b], r[a]);
        else  // a <= c < b
            cswap(r[b], r[c]);
    }
    assert(r[a] <= r[b] && r[b] <= r[c] || r[a] >= r[b] && r[b] >= r[c]);
}

/**
Sorts in place r[a], r[b], and r[c].
*/
template <class T>
void sort3(T* r, size_t a, size_t b, size_t c)
{
    if (r[b] <CNT r[a]) // b < a
    {
        if (r[c] <CNT r[b]) // c < b < a
        {
            cswap(r[a], r[c]); // a < b < c
        }
        else // b < a, b <= c
        {
            auto t = r[a];
            r[a] = r[b];
            if (r[c] <CNT t) // b <= c < a
            {
                r[b] = r[c];
                r[c] = t;
            }
            else // b < a <= c
            {
                r[b] = t;
            }
        }
    }
    else if (r[c] <CNT r[b]) // a <= b, c < b
    {
        auto t = r[c];
        r[c] = r[b];
        if (t <CNT r[a]) // c < a < b
        {
            r[b] = r[a];
            r[a] = t;
        }
        else // a <= c < b
        {
            r[b] = t;
        }
    }

    assert(r[a] <= r[b] && r[b] <= r[c]);
}

/**
If leanRight == false, swaps the lower median of r[a]...r[d] into r[b] and
the minimum into r[a]. If leanRight == true, swaps the upper median of
r[a]...r[d] into r[c] and the minimum into r[d].
*/
template <bool leanRight, class T>
void partition4(T* r, size_t a, size_t b, size_t c, size_t d)
{
    assert(a != b && a != c && a != d && b != c && b != d
        && c != d);
    /* static */ if (leanRight)
    {
        // In the median of 5 algorithm, consider r[e] infinite
        if (r[c] <CNT r[a]) {
            cswap(r[a], r[c]);
        } // a <= c
        if (r[d] <CNT r[b]) {
            cswap(r[b], r[d]);
        } // a <= c, b <= d
        if (r[d] <CNT r[c]) {
            cswap(r[c], r[d]); // a <= d, b <= c < d
            cswap(r[a], r[b]); // b <= d, a <= c < d
        } // a <= c <= d, b <= d
        if (r[c] <CNT r[b]) { // a <= c <= d, c < b <= d
            cswap(r[b], r[c]); // a <= b <= c <= d
        } // a <= b <= c <= d
        assert(r[a] <= r[c] && r[b] <= r[c] && r[c] <= r[d]);
    }
    else
    {
        // In the median of 5 algorithm consider r[a] infinitely small, then
        // change b->a. c->b, d->c, e->d
        if (r[c] <CNT r[a]) {
            cswap(r[a], r[c]);
        }
        if (r[c] <CNT r[b]) {
            cswap(r[b], r[c]);
        }
        if (r[d] <CNT r[a]) {
            cswap(r[a], r[d]);
        }
        if (r[d] <CNT r[b]) {
            cswap(r[b], r[d]);
        } else {
            if (r[b] <CNT r[a]) {
                cswap(r[a], r[b]);
            }
        }
        assert(r[a] <= r[b] && r[b] <= r[c] && r[b] <= r[d]);
    }
}

/**
Places the median of r[a]...r[e] in r[c] and partitions the other elements
around it.
*/
template <class T>
void partition5(T* r, size_t a, size_t b, size_t c, size_t d, size_t e)
{
    assert(a != b && a != c && a != d && a != e && b != c && b != d && b != e
        && c != d && c != e && d != e);
    if (r[c] <CNT r[a]) {
        cswap(r[a], r[c]);
    }
    if (r[d] <CNT r[b]) {
        cswap(r[b], r[d]);
    }
    if (r[d] <CNT r[c]) {
        cswap(r[c], r[d]);
        cswap(r[a], r[b]);
    }
    if (r[e] <CNT r[b]) {
        cswap(r[b], r[e]);
    }
    if (r[e] <CNT r[c]) {
        cswap(r[c], r[e]);
        if (r[c] <CNT r[a]) {
            cswap(r[a], r[c]);
        }
    } else {
        if (r[c] <CNT r[b]) {
            cswap(r[b], r[c]);
        }
    }
    assert(r[a] <= r[c] && r[b] <= r[c] && r[c] <= r[d] && r[c] <= r[e]);
}

/**
Implements Hoare partition.
*/
template <class T>
T* pivotPartition(T* r, size_t k, size_t length)
{
    assert(k < length);
    cswap(*r, r[k]);
    size_t lo = 1, hi = length - 1;
    for (;; ++lo, --hi)
    {
        for (;; ++lo)
        {
            if (lo > hi) goto loop_done;
            if (r[lo] >=CNT *r) break;
        }
        // found the left bound:  r[lo] >= r[0]
        assert(lo <= hi);
        for (; *r <CNT r[hi]; --hi)
        {
        }
        if (lo >= hi) break;
        // found the right bound: r[hi] <= r[0], swap & make progress
        assert(r[lo] >= r[hi]);
        cswap(r[lo], r[hi]);
    }
loop_done:
    --lo;
    cswap(r[lo], *r);
    return r + lo;
}

/**
Implements the quickselect algorithm, parameterized with a partition function.
*/
template <class T, T* (*partition)(T*, T*)>
void quickselect(T* r, T* mid, T* end)
{
    if (r == end || mid >= end) return;
    assert(r <= mid && mid < end);
    for (;;) switch (end - r)
    {
    case 1:
        return;
    case 2:
        if (*r >CNT r[1]) cswap(*r, r[1]);
        return;
    case 3:
        sort3(r, 0, 1, 2);
        return;
    case 4:
        switch (mid - r)
        {
            case 0: goto select_min;
            case 1: partition4<false>(r, 0, 1, 2, 3); break;
            case 2: partition4<true>(r, 0, 1, 2, 3); break;
            case 3: goto select_max;
            default: assert(false);
        }
        return;
    default:
        assert(end - r > 4);
        if (r == mid)
        {
        select_min:
            auto pivot = r;
            for (++mid; mid < end; ++mid)
                if (*mid <CNT *pivot) pivot = mid;
            cswap(*r, *pivot);
            return;
        }
        if (mid + 1 == end)
        {
        select_max:
            auto pivot = r;
            for (mid = r + 1; mid < end; ++mid)
                if (*pivot <CNT *mid) pivot = mid;
            cswap(*pivot, end[-1]);
            return;
        }
        auto pivot = partition(r, end);
        if (pivot == mid) return;
        if (pivot > mid)
        {
            end = pivot;
        }
        else
        {
            r = pivot + 1;
        }
    }
}

/**
Returns the index of the median of r[a], r[b], and r[c] without writing
anything.
*/
template <class T>
size_t medianIndex(const T* r, size_t a, size_t b, size_t c)
{
    if (r[a] >CNT r[c]) std::swap(a, c);
    if (r[b] >CNT r[c]) return c;
    if (r[b] <CNT r[a]) return a;
    return b;
}

/**
Returns the index of the median of r[a], r[b], r[c], and r[d] without writing
anything. If leanRight is true, computes the upper median. Otherwise, conputes
the lower median.
*/
template <bool leanRight, class T>
static size_t medianIndex(T* r, size_t a, size_t b, size_t c, size_t d)
{
    if (r[d] <CNT r[c]) std::swap(c, d);
    assert(r[c] <= r[d]);
    /* static */ if (leanRight)
    {
        if (r[c] <CNT r[a])
        {
            assert(r[c] < r[a] && r[c] <= r[d]); // so r[c] is out
            return medianIndex(r, a, b, d);
        }
        assert(r[a] <= r[c] && r[a] <= r[d]); // so r[a] is out
    }
    else
    {
        if (r[d] >=CNT r[a])
        {
            assert(r[d] >= r[c] && r[d] >= r[a]); // so r[d] is out
            return medianIndex(r, a, b, c);
        }
        assert(r[a] > r[d] && r[a] > r[c]); // so r[a] is out
    }
    // Could return medianIndex(r, b, c, d) but we already know r[c] <= r[d]
    if (r[b] <=CNT r[c]) return c;
    if (r[b] >CNT r[d]) return d;
    return b;
}

/**
Tukey's Ninther: compute the median of r[_1], r[_2], r[_3], then the median of
r[_4], r[_5], r[_6], then the median of r[_7], r[_8], r[_9], and then swap the
median of those three medians into r[_5].
*/
template <class T>
void ninther(T* r, size_t _1, size_t _2, size_t _3, size_t _4, size_t _5,
    size_t _6, size_t _7, size_t _8, size_t _9)
{
    _2 = medianIndex(r, _1, _2, _3);
    _8 = medianIndex(r, _7, _8, _9);
    if (r[_2] >CNT r[_8]) std::swap(_2, _8);
    if (r[_4] >CNT r[_6]) std::swap(_4, _6);
    // Here we know that r[_2] and r[_8] are the other two medians and that
    // r[_2] <= r[_8]. We also know that r[_4] <= r[_6]
    if (r[_5] <CNT r[_4])
    {
        // r[_4] is the median of r[_4], r[_5], r[_6]
    }
    else if (r[_5] >CNT r[_6])
    {
        // r[_6] is the median of r[_4], r[_5], r[_6]
        _4 = _6;
    }
    else
    {
        // Here we know r[_5] is the median of r[_4], r[_5], r[_6]
        if (r[_5] <CNT r[_2]) return cswap(r[_5], r[_2]);
        if (r[_5] >CNT r[_8]) return cswap(r[_5], r[_8]);
        // This is the only path that returns with no swap
        return;
    }
    // Here we know r[_4] is the median of r[_4], r[_5], r[_6]
    if (r[_4] <CNT r[_2]) _4 = _2;
    else if (r[_4] >CNT r[_8]) _4 = _8;
    cswap(r[_5], r[_4]);
}

/**
Input assumptions:

(a) hi <= rite
(c) the range r[0 .. hi] contains elements no smaller than r[0]

Output guarantee: same as Hoare partition using r[0] as pivot. Returns the new
position of the pivot.
*/
template <class T>
size_t expandPartitionRight(T* r, size_t hi, size_t rite)
{
    size_t pivot = 0;
    assert(pivot <= hi);
    assert(hi <= rite);
    // First loop: spend r[pivot .. hi]
    for (; pivot < hi; --rite)
    {
        if (rite == hi) goto done;
        if (r[rite] >=CNT r[0]) continue;
        ++pivot;
        assert(r[pivot] >= r[0]);
        cswap(r[rite], r[pivot]);
    }
    // Second loop: make left and pivot meet
    for (; rite > pivot; --rite)
    {
        if (r[rite] >=CNT r[0]) continue;
        while (rite > pivot)
        {
            ++pivot;
            if (r[0] <CNT r[pivot])
            {
                cswap(r[rite], r[pivot]);
                break;
            }
        }
    }

done:
    cswap(r[0], r[pivot]);
    return pivot;
}

/**
Input assumptions:

(a) lo > 0, lo <= pivot
(b) the range r[lo .. pivot] already contains elements no greater than r[pivot]

Output guarantee: Same as Hoare partition around r[pivot]. Returns the new
position of the pivot.

*/
template <class T>
size_t expandPartitionLeft(T* r, size_t lo, size_t pivot)
{
    assert(lo > 0 && lo <= pivot);
    size_t left = 0;
    const auto oldPivot = pivot;
    for (; lo < pivot; ++left)
    {
        if (left == lo) goto done;
        if (r[oldPivot] >=CNT r[left]) continue;
        --pivot;
        assert(r[oldPivot] >= r[pivot]);
        cswap(r[left], r[pivot]);
    }
    // Second loop: make left and pivot meet
    for (;; ++left)
    {
        if (left == pivot) break;
        if (r[oldPivot] >=CNT r[left]) continue;
        for (;;)
        {
            if (left == pivot) goto done;
            --pivot;
            if (r[pivot] <CNT r[oldPivot])
            {
                cswap(r[left], r[pivot]);
                break;
            }
        }
    }

done:
    cswap(r[oldPivot], r[pivot]);
    return pivot;
}

/**
Input assumptions:

(a) lo <= pivot, pivot < hi, hi <= length
(b) the range r[lo .. pivot] already contains elements no greater than
r[pivot]
(c) the range r[pivot .. hi] already contains elements no smaller than
r[pivot]

Output guarantee: Same as Hoare partition around r[pivot], returning the new
position of the pivot.
*/
template <class T>
size_t expandPartition(T* r, size_t lo, size_t pivot, size_t hi, size_t length)
{
    assert(lo <= pivot && pivot < hi && hi <= length);
    --hi;
    --length;
    size_t left = 0;
    for (;; ++left, --length)
    {
        for (;; ++left)
        {
            if (left == lo)
                return pivot +
                    expandPartitionRight(r + pivot, hi - pivot, length - pivot);
            if (r[left] >CNT r[pivot]) break;
        }
        for (;; --length)
        {
            if (length == hi)
                return left +
                    expandPartitionLeft(r + left, lo - left, pivot - left);
            if (r[pivot] >=CNT r[length]) break;
        }
        cswap(r[left], r[length]);
    }
}
