/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <random>
#include <sys/stat.h>
#include "timer.h"
using namespace std;

extern void (*computeSelection)(double*, double*, double*);
const size_t epochs = 21;
#ifdef COUNT_SWAPS
unsigned long g_swaps = 0;
#endif
#ifdef COUNT_WASTED_SWAPS
unsigned long g_wastedSwaps = 0;
#endif

int main(int argc, char** argv)
{
    if (argc != 2) return 1;
    // Is this file random? If so, we shoulr reshuffle after each run
    const bool reshuffle = strstr(argv[1], "random") != nullptr;

    // Load data from input file
    struct stat stat_buf;
    if (stat(argv[1], &stat_buf) != 0) return 2;
    if (stat_buf.st_size == 0 || stat_buf.st_size % 8 != 0) return 3;
    const size_t dataLen = stat_buf.st_size / 8;
    auto data = new double[dataLen];
    const auto f = fopen(argv[1], "rb");
    if (!f) return 4;
    if (fread(data, sizeof(double), dataLen, f) != dataLen) return 5;
    if (fclose(f) != 0) return 6;

    // The fraction we're searching for
    const size_t frac = 2;
    // The statistic order we're looking for
    size_t index = dataLen / frac;

    std::random_device rd;
    std::mt19937 g(1);

    double durations[epochs];
    double median = 0;

    for (size_t i = 0; ; ++i)
    {
        vector<double> v {data, data + dataLen};
        auto b = &v[0];
#ifdef COUNT_SWAPS
        g_swaps = 0;
#endif
#ifdef COUNT_WASTED_SWAPS
        g_wastedSwaps = 0;
#endif
        //////////////////// TIMING {
        Timer t;
        (*computeSelection)(b, b + index, b + dataLen);
        durations[i] = t.elapsed();
        //////////////////// } TIMING
        if (median == 0)
        {
            median = v[index];
        }
        else
        {
            if (median != v[index]) return 7;
        }
        if (++i == epochs)
        {
            fprintf(stderr, "size: %lu\nmedian: %g\n", dataLen, median);
            if (reshuffle) fprintf(stderr, "shuffled: 1\n");
#ifdef COUNT_SWAPS
            fprintf(stderr, "swaps: %lu\n", g_swaps);
#endif
#ifdef COUNT_WASTED_SWAPS
            fprintf(stderr, "wasted_swaps: %lu\n", g_wastedSwaps);
#endif
            break;
        }
        if (reshuffle)
            shuffle(data, data + dataLen, g);
    }

    // Verify
    vector<double> v {data, data + dataLen};
    sort(v.begin(), v.end());
    if (median != v[index]) return 8;

    // Print results
    sort(durations, durations + epochs);
    double sum = 0;
    for (size_t i = 0; i < epochs - 2; ++i) sum += durations[i];
    printf("%g\n", sum / (epochs - 2));
}
