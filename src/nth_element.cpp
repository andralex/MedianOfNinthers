/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

struct Double
{
private:
    double payload;

public:
    friend inline bool operator<(const Double& a, const Double& b)
    {
#ifdef COUNT_COMPARISONS
        extern unsigned long g_comparisons;
        ++g_comparisons;
#endif
        return a.payload < b.payload;
    }

    void swap(Double & b)
    {
#ifdef COUNT_SWAPS
        extern unsigned long g_swaps;
        ++g_swaps;
#endif
#ifdef COUNT_WASTED_SWAPS
        extern unsigned long g_wastedSwaps;
        if (payload == b.payload) ++g_wastedSwaps;
#endif
        auto t = payload;
        payload = b.payload;
        b.payload = t;
    }
};

// So sue me
namespace std
{
    inline void swap(Double& a, Double& b)
    {
        a.swap(b);
    }
}

#include <algorithm>

void nth_element(double* b, double* m, double* e)
{
    return std::nth_element((Double*)b, (Double*)m, (Double*)e);
}

void (*computeSelection)(double*, double*, double*)
    = &nth_element;
