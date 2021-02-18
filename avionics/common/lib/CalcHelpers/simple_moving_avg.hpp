#pragma once

#include <array>
#include <cmath>

/**
 * \tparam T Underlying floating point type to be used.
 * \tparam size Size of the buffer to be used. Should be at least 2; otherwise
 * the variance and standard deviation calculations lead to division by zero.
 */
template <typename T, int size> class SimpleMovingAvg {
    static_assert(size > 0, "Moving average buffer size must be > 0");

  public:
    /**
     * @brief Constructor - although this directly defers to reset() for code
     * reuse
     * @param init Initial value
     * @param var Initial variance.
     * @param alpha Alpha to use. One common recommendation is set alpha = 2/(N
     * + 1) where N is approximate data set size for equivalent simple moving
     * average.
     */
    SimpleMovingAvg(T const init)
        : total_(init * size), avg_(init), position_(data_.begin()) {
        for (auto &i : data_) {
            i = init;
        }
    }

    /**
     * @brief Adds a new value to the moving average.
     * @param new_dat New data to add.
     * @return New average value.
     */
    T addValue(T const &new_dat) {
        total_ -= *position_;
        total_ += new_dat;
        *position_ = new_dat;

        ++position_;
        if (position_ == data_.end()) {
            position_ = data_.begin();
        }

        avg_ = total_ / size;
        return avg_;
    }

    /**
     * @brief Get current average.
     * @return Average value.
     */
    T getAverage() const { return avg_; }

    /**
     * @brief Get current sample variance.
     * @return Variance.
     */
    T getVariance() const {
        T sum = 0;
        for (auto i : data_) {
            sum += (i - avg_) * (i - avg_);
        }
        return sum / (size - 1);
    }

    /**
     * @brief Get current standard deviation.
     * @return Standard deviation.
     */
    T getStandardDeviation() const { return std::sqrt(getVariance()); }

  private:
    typedef std::array<T, size> data_t;

    data_t data_;
    T total_;
    T avg_;
    typename data_t::iterator position_;
};
