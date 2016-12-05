/*          Copyright Andrei Alexandrescu, 2016-.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://boost.org/LICENSE_1_0.txt)
 */

#pragma once
#include <chrono>
#include <ratio>

class Timer
{
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const
    {
        return std::chrono::duration_cast<ms_>
            (clock_::now() - beg_).count();
    }

private:
    using clock_ = std::chrono::high_resolution_clock;
    using second_ = std::chrono::duration<double, std::ratio<1>>;
    using ms_ = std::chrono::duration<double, std::milli>;
    std::chrono::time_point<clock_> beg_;
};
