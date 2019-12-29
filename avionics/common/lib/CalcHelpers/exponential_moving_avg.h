#ifndef CALC_HELPERS_EXPONENTIAL_MOVING_AVG_H_C9C3AE02B2AF4468AB08A70F9B28D99F
#define CALC_HELPERS_EXPONENTIAL_MOVING_AVG_H_C9C3AE02B2AF4468AB08A70F9B28D99F
/*Classes--------------------------------------------------------------*/
//https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
//T should be some floating point type.
template <typename T>
class ExponentialMovingAvg {
public:
    /**
     * @brief Constructor
     * @param init Initial value
     * @param alpha Alpha to use. One common recommendation is set alpha = 2/(N + 1) where N is approximate data set size for equivalent simple moving average.
     */
    ExponentialMovingAvg(T const init, T const alpha) :
        m_alpha(alpha), m_average(init) {}

    /**
     * @brief Adds a new value to the moving average.
     * @param new_dat New data to add.
     * @return New average value.
     */
    T addValue(T const &new_dat) {
        return m_average = m_alpha * new_dat + m_average * (1 - m_alpha);
    }

    /**
     * @brief Get current average.
     * @return Average value.
     */
    T getAverage() const {return m_average;}

private:
    T m_alpha;
    T m_average;
};

#endif
