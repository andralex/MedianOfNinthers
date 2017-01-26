/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#pragma once
#include <algorithm>
#include <cassert>

/**
Instrumented swap
*/
#ifdef COUNT_SWAPS
extern unsigned long g_swaps;
#endif
#ifdef COUNT_WASTED_SWAPS
extern unsigned long g_wastedSwaps;
#endif

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
Swaps the median of r[a], r[b], and r[c] into r[b].
*/
template <class T>
void median3(T* r, size_t a, size_t b, size_t c)
{
    if (r[b] < r[a]) // b < a
    {
        if (r[b] < r[c]) // b < a, b < c
        {
            if (r[c] < r[a]) // b < c < a
                cswap(r[b], r[c]);
            else  // b < a <= c
                cswap(r[b], r[a]);
        }
    }
    else if (r[c] < r[b]) // a <= b, c < b
    {
        if (r[c] < r[a]) // c < a <= b
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
    if (r[b] < r[a]) // b < a
    {
        if (r[c] < r[b]) // c < b < a
        {
            cswap(r[a], r[c]); // a < b < c
        }
        else // b < a, b <= c
        {
            auto t = r[a];
            r[a] = r[b];
            if (r[c] < t) // b <= c < a
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
    else if (r[c] < r[b]) // a <= b, c < b
    {
        auto t = r[c];
        r[c] = r[b];
        if (t < r[a]) // c < a < b
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
        if (r[c] < r[a]) {
    		cswap(r[a], r[c]);
    	} // a <= c
    	if (r[d] < r[b]) {
    		cswap(r[b], r[d]);
    	} // a <= c, b <= d
    	if (r[d] < r[c]) {
    		cswap(r[c], r[d]); // a <= d, b <= c < d
    		cswap(r[a], r[b]); // b <= d, a <= c < d
    	} // a <= c <= d, b <= d
		if (r[c] < r[b]) { // a <= c <= d, c < b <= d
			cswap(r[b], r[c]); // a <= b <= c <= d
    	} // a <= b <= c <= d
        assert(r[a] <= r[c] && r[b] <= r[c] && r[c] <= r[d]);
    }
    else
    {
        // In the median of 5 algorithm consider r[a] infinitely small, then
        // change b->a. c->b, d->c, e->d
    	if (r[c] < r[a]) {
    		cswap(r[a], r[c]);
    	}
    	if (r[c] < r[b]) {
    		cswap(r[b], r[c]);
    	}
    	if (r[d] < r[a]) {
    		cswap(r[a], r[d]);
    	}
    	if (r[d] < r[b]) {
    		cswap(r[b], r[d]);
    	} else {
    		if (r[b] < r[a]) {
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
    if (r[c] < r[a]) {
		cswap(r[a], r[c]);
	}
	if (r[d] < r[b]) {
		cswap(r[b], r[d]);
	}
	if (r[d] < r[c]) {
		cswap(r[c], r[d]);
		cswap(r[a], r[b]);
	}
	if (r[e] < r[b]) {
		cswap(r[b], r[e]);
	}
	if (r[e] < r[c]) {
		cswap(r[c], r[e]);
		if (r[c] < r[a]) {
			cswap(r[a], r[c]);
		}
	} else {
		if (r[c] < r[b]) {
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
            if (r[lo] >= *r) break;
        }
        // found the left bound:  r[lo] >= r[0]
        assert(lo <= hi);
        for (; *r < r[hi]; --hi)
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
        if (*r > r[1]) cswap(*r, r[1]);
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
                if (*mid < *pivot) pivot = mid;
            std::iter_swap(r, pivot);
            return;
        }
        if (mid + 1 == end)
        {
        select_max:
            auto pivot = r;
            for (mid = r + 1; mid < end; ++mid)
                if (*pivot < *mid) pivot = mid;
            std::iter_swap(pivot, end - 1);
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
size_t median3Index(const T* r, size_t a, size_t b, size_t c)
{
    if (r[b] < r[a]) // b < a
    {
        if (r[b] < r[c]) // b < a, b < c
        {
            return r[c] < r[a] ? c : a;
        }
    }
    else if (r[c] < r[b]) // a <= b, c < b
    {
        return r[c] < r[a] ? a : c;
    }
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
    cswap(r[_5], r[median3Index(r,
        median3Index(r, _1, _2, _3),
        median3Index(r, _4, _5, _6),
        median3Index(r, _7, _8, _9))
    ]);
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
        if (r[rite] >= r[0]) continue;
        ++pivot;
        assert(r[pivot] >= r[0]);
        cswap(r[rite], r[pivot]);
    }
    // Second loop: make left and pivot meet
    for (; rite > pivot; --rite)
    {
        if (r[rite] >= r[0]) continue;
        while (rite > pivot)
        {
            ++pivot;
            if (r[0] < r[pivot])
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
        if (r[oldPivot] >= r[left]) continue;
        --pivot;
        assert(r[oldPivot] >= r[pivot]);
        cswap(r[left], r[pivot]);
    }
    // Second loop: make left and pivot meet
    for (;; ++left)
    {
        if (left == pivot) break;
        if (r[oldPivot] >= r[left]) continue;
        for (;;)
        {
            if (left == pivot) goto done;
            --pivot;
            if (r[pivot] < r[oldPivot])
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
            if (r[left] > r[pivot]) break;
        }
        for (;; --length)
        {
            if (length == hi)
                return left +
                    expandPartitionLeft(r + left, lo - left, pivot - left);
            if (r[pivot] >= r[length]) break;
        }
        cswap(r[left], r[length]);
    }
}
