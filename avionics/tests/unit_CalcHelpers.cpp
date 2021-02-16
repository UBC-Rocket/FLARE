#include "gtest/gtest.h"

#include <cmath>
#include <vector>

#include "CalcHelpers/exponential_moving_avg.hpp"
#include "CalcHelpers/simple_moving_avg.hpp"

TEST(ExponentialMovingAvg, Basic) {
    ExponentialMovingAvg<float> avg(0, 0, 0.5);
    avg.addValue(1);
    EXPECT_FLOAT_EQ(avg.getAverage(), 0.5);
    EXPECT_FLOAT_EQ(avg.getVariance(), 0.25);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), std::sqrt((0.25)));

    avg.addValue(0);
    EXPECT_FLOAT_EQ(avg.getAverage(), 0.25);
    EXPECT_FLOAT_EQ(avg.getVariance(), 0.1875);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), std::sqrt((0.1875)));

    avg.addValue(1);
    EXPECT_FLOAT_EQ(avg.getAverage(), 0.625);
    EXPECT_FLOAT_EQ(avg.getVariance(), 0.234375);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), std::sqrt((0.234375)));

    avg.addValue(2);
    EXPECT_FLOAT_EQ(avg.getAverage(), 1.3125);
    EXPECT_FLOAT_EQ(avg.getVariance(), 0.58984375);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), std::sqrt((0.58984375)));
}

TEST(ExponentialMovingAvg, Application) {
    ExponentialMovingAvg<float> avg(0, 0, 0.05);
    std::vector<float> data{
        10.104, 9.85,  10.285, 9.817,  10.361, 10.266, 9.743,  10.299, 10.006,
        10.129, 9.836, 10.296, 10.356, 10.432, 9.793,  10.209, 9.766,  9.865,
        9.843,  10.49, 9.629,  10.355, 9.946,  9.873,  9.794,  9.965,  10.095,
        10.232, 9.57,  9.865,  10.139, 10.439, 9.57,   9.793,  9.837,  9.905,
        10.253, 9.803, 10.412, 9.535,  10.079, 10.037, 10.31,  10.365, 10.156,
        9.614,  9.77,  9.678,  9.993,  9.823,  9.895,  10.145, 10.206, 9.518,
        9.576,  9.973, 10.191, 9.665,  10.413, 9.593,  9.669,  10.035, 10.31,
        10.084, 9.725, 9.851,  9.872,  10.35,  9.822,  10.438, 9.672,  10.024,
        9.906,  9.523, 10.488, 9.842,  9.574,  10.257, 9.764,  9.677,  10.451,
        9.72,   9.924, 10.441, 9.67,   9.768,  9.795,  10.189, 9.971,  9.931,
        9.709,  9.946, 9.53,   10.381, 10.115, 10.071, 10.135, 9.788,  9.602,
        10.402};
    for (float i : data) {
        avg.addValue(i);
    }
    EXPECT_NEAR(avg.getAverage(), 10, 0.1);
}

TEST(SimpleMovingAvg, Basic) {
    SimpleMovingAvg<float, 4> avg(5);
    EXPECT_FLOAT_EQ(avg.getAverage(), 5);
    EXPECT_FLOAT_EQ(avg.getVariance(), 0);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), 0);

    avg.addValue(9);
    EXPECT_FLOAT_EQ(avg.getAverage(), 6);
    EXPECT_FLOAT_EQ(avg.getVariance(), 4);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), 2);

    avg.addValue(-2);
    EXPECT_FLOAT_EQ(avg.getAverage(), 4.25);
    EXPECT_FLOAT_EQ(avg.getVariance(), 20.916666666666668);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), 4.573474244670748);

    avg.addValue(5.5);
    EXPECT_FLOAT_EQ(avg.getAverage(), 4.375);
    EXPECT_FLOAT_EQ(avg.getVariance(), 21.229166666666668);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), 4.6075119822596955);

    avg.addValue(5);
    EXPECT_FLOAT_EQ(avg.getAverage(), 4.375);
    EXPECT_FLOAT_EQ(avg.getVariance(), 21.229166666666668);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), 4.6075119822596955);

    avg.addValue(9);
    EXPECT_FLOAT_EQ(avg.getAverage(), 4.375);
    EXPECT_FLOAT_EQ(avg.getVariance(), 21.229166666666668);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), 4.6075119822596955);

    avg.addValue(7);
    EXPECT_FLOAT_EQ(avg.getAverage(), 6.625);
    EXPECT_FLOAT_EQ(avg.getVariance(), 3.2291666666666665);
    EXPECT_FLOAT_EQ(avg.getStandardDeviation(), 1.796988221070652);
}

TEST(SimpleMovingAvg, Basic2) {
    SimpleMovingAvg<float, 2> avg(0);
    avg.addValue(-10);
    avg.addValue(-9);
    for (int i = -8; i < 10; ++i) {
        avg.addValue(i);
        EXPECT_FLOAT_EQ(avg.getAverage(), i - 0.5);
    }
}

TEST(SimpleMovingAvg, Application) {
    SimpleMovingAvg<float, 20> avg(0);
    std::vector<float> data{
        10.104, 9.85,  10.285, 9.817,  10.361, 10.266, 9.743,  10.299, 10.006,
        10.129, 9.836, 10.296, 10.356, 10.432, 9.793,  10.209, 9.766,  9.865,
        9.843,  10.49, 9.629,  10.355, 9.946,  9.873,  9.794,  9.965,  10.095,
        10.232, 9.57,  9.865,  10.139, 10.439, 9.57,   9.793,  9.837,  9.905,
        10.253, 9.803, 10.412, 9.535,  10.079, 10.037, 10.31,  10.365, 10.156,
        9.614,  9.77,  9.678,  9.993,  9.823,  9.895,  10.145, 10.206, 9.518,
        9.576,  9.973, 10.191, 9.665,  10.413, 9.593,  9.669,  10.035, 10.31,
        10.084, 9.725, 9.851,  9.872,  10.35,  9.822,  10.438, 9.672,  10.024,
        9.906,  9.523, 10.488, 9.842,  9.574,  10.257, 9.764,  9.677,  10.451,
        9.72,   9.924, 10.441, 9.67,   9.768,  9.795,  10.189, 9.971,  9.931,
        9.709,  9.946, 9.53,   10.381, 10.115, 10.071, 10.135, 9.788,  9.602,
        10.402};
    for (float i : data) {
        avg.addValue(i);
    }
    EXPECT_NEAR(avg.getAverage(), 10, 0.1);
}
