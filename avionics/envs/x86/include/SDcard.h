#ifndef SDcard_h
#define SDcard_h

#include <iostream>
#include <fstream>


class NativeSDcard {
public:
    bool init(const char* filename) {
        std::string name = filename;
        name += ".csv";
        m_file.open(name);
        return true;
    }
    NativeSDcard() {
        m_file.exceptions(std::ofstream::failbit | std::ofstream::badbit );
    }

    ~NativeSDcard() {
        m_file.close();
    }

    template<typename T>
    void print(T t) {
        m_file << t;
    }

    template<typename T>
    void println(T t) {
        m_file << t << "\n";
    }

    void flush() {
        m_file.flush();
    }

private:
    std::ofstream m_file;
};

#endif /* SDcard_h */
