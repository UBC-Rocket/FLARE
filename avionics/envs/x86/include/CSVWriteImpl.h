#pragma once

class CSVWriteImpl { // TODO - make this not full of no-ops
  public:
    bool init(char const *filename) { return true; }

    /**
     * @brief writes t to the next csv column in order
     * @param t the data to write
     */
    template <typename T> void print(T t) {}

    /**
     * @brief writes t and ends the line
     * @param t the data to write
     */
    template <typename T> void println(T t) {}

    /**
     * @brief flushes the cached data to the SD card
     */
    void flush() {}
};
