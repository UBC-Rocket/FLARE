#ifndef CALC_HELPERS_EXPONENTIAL_MOVING_AVG_H_C9C3AE02B2AF4468AB08A70F9B28D99F
#define CALC_HELPERS_EXPONENTIAL_MOVING_AVG_H_C9C3AE02B2AF4468AB08A70F9B28D99F

#include <cmath>
/*Classes--------------------------------------------------------------*/
//https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
//T should be some floating point type, since a number of functions have their parameters passed in by value and return by value as well.
template <typename T>
class ExponentialMovingAvg {
public:
    /**
     * @brief Constructor - although this directly defers to reset() for code reuse
     * @param init Initial value
     * @param var Initial variance.
     * @param alpha Alpha to use. One common recommendation is set alpha = 2/(N + 1) where N is approximate data set size for equivalent simple moving average.
     */
    ExponentialMovingAvg(T const init, T const var, T const alpha) {
        reset(init, var, alpha);
    }

    ExponentialMovingAvg(){} //Dummy constructor that doesn't do anything - must call reset later.

    /**
     * @brief Resets the averager, reinitializing all relevant variables
     * @param init Initial value
     * @param var Initial variance.
     * @param alpha Alpha to use. One common recommendation is set alpha = 2/(N + 1) where N is approximate data set size for equivalent simple moving average.
     */
    void reset(T const init, T const var, T const alpha){
        m_avg = init;
        m_var = var;
        m_alpha = alpha;
        m_SD = std::sqrt(m_var);
    }

    /**
     * @brief Adds a new value to the moving average.
     * @param new_dat New data to add.
     * @return New average value.
     */
    T addValue(T const &new_dat) {
        T delta = new_dat - m_avg;
        m_avg = m_avg + m_alpha * delta;
        m_var = (1 - m_alpha) * (m_var + m_alpha * delta * delta);
        m_SD = std::sqrt(m_var);
        return m_avg;
    }

    /**
     * @brief Get current average.
     * @return Average value.
     */
    T getAverage() const {return m_avg;}

    /**
     * @brief Get current variance.
     * @return Variance.
     */
    T getVariance() const {return m_var;}

    /**
     * @brief Get current standard deviation.
     * @return Standard deviation.
     */
    T getStandardDeviation() const {return m_SD;}

private:
    T m_alpha;
    T m_avg; //average
    T m_var; //variance
    T m_SD; //standard deviation
};

#endif
