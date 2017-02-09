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
#ifdef COUNT_SWAPS
unsigned long g_swaps = 0;
#endif
#ifdef COUNT_WASTED_SWAPS
unsigned long g_wastedSwaps = 0;
#endif
#ifdef COUNT_COMPARISONS
unsigned long g_comparisons = 0;
#endif

// Compute standard deviation (pass by value is intentional)
double stddev(const double* b, double* e, double avg)
{
    const double size = e - b;
    double result = 0;
    for (; b != e; ++b)
    {
        auto x = *b - avg;
        result += x * x;
    }
    return sqrt(result / size);
}

int main(int argc, char** argv)
{
    if (argc != 2) return 1;

    // Is this file random? If so, we shoulr randomInput after each run
    const bool randomInput = strstr(argv[1], "random") != nullptr;

    // Figure out how many epochs we need
#ifdef MEASURE_TIME
    const size_t epochs = 51;
    const size_t outlierEpochs = 2;
#else
    const size_t epochs = 1;
    const size_t outlierEpochs = 0;
#endif

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

    // The fraction we're searching for (2 for median)
    const size_t frac = 2;
    // The order statistic we're looking for
    size_t index = dataLen / frac;

    std::random_device rd;
    std::mt19937 g(1);

    double durations[epochs];
    double median = 0;

    for (size_t i = 0; ; ++i)
    {
        vector<double> v {data, data + dataLen};
        auto b = &v[0];

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
            break;
        }
        if (randomInput)
            shuffle(data, data + dataLen, g);
    }

    // Verify
    vector<double> v {data, data + dataLen};
    sort(v.begin(), v.end());
    if (median != v[index]) return 8;

    // Print results
    sort(durations, durations + epochs);
    double avg = 0;
    for (size_t i = 0; i < epochs - outlierEpochs; ++i) avg += durations[i];
    avg /= epochs - outlierEpochs;

#ifdef MEASURE_TIME
    printf("times:");
    for (size_t i = 0; i < epochs - outlierEpochs; ++i)
    {
        printf(" %g", durations[i]);
    }
    printf("\n");
    printf("milliseconds: %g\n", avg);
    printf("stddev: %g\n",
        stddev(durations, durations + epochs - outlierEpochs, avg));
#endif
    printf("size: %lu\nmedian: %g\n", dataLen, median);
    if (randomInput) printf("shuffled: 1\n");
#ifdef COUNT_COMPARISONS
    printf("comparisons: %g\n", double(g_comparisons) / (epochs * dataLen));
#endif
#ifdef COUNT_SWAPS
    printf("swaps: %g\n", double(g_swaps) / (epochs * dataLen));
#endif
#ifdef COUNT_WASTED_SWAPS
    printf("wasted_swaps: %g\n", double(g_wastedSwaps) / (epochs * dataLen));
#endif
}
