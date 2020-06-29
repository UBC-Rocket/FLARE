#pragma once

#include <fstream>
#include <iostream>

class CSVWriteImpl {
  public:
    ~CSVWriteImpl() { file_.close(); }

    /**
     * @brief Delayed constructor.
     * @param filename Name of the file to open.
     * @return True on successful file opening.
     */
    bool init(char const *filename) {
        file_.open(filename);
        return static_cast<bool>(file_);
    }

    /**
     * @brief writes t to the next csv column in order
     * @param t the data to write
     */
    template <typename T> void print(T t) { file_ << t; }

    /**
     * @brief writes t and ends the line
     * @param t the data to write
     */
    template <typename T> void println(T t) { file_ << t << "\n"; }

    /**
     * @brief flushes the cached data to the SD card
     */
    void flush() { file_.flush(); }

  private:
    std::ofstream file_;
};
