/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#include "instrumented_double.h"

void nth_element(double* b, double* m, double* e)
{
    return std::nth_element((Double*)b, (Double*)m, (Double*)e);
}

void (*computeSelection)(double*, double*, double*)
    = &nth_element;
