#ifndef SDcard_h
#define SDcard_h

#include <iostream>
#include <fstream>


class SDcard {
public:
    bool init(const char* filename) {
        std::string name = filename;
        name += ".csv";
        myFile.open(name);
        return true;
    }
    
    ~SDcard() {
        myFile.close();
    }
    
    void print(std::string str) {
        myFile << str;
    }
    
    void println(std::string str) {
        myFile << str << "\n";
    }
    
    void flush() {
        
    }
    
private:
    std::ofstream myFile;
};

#endif /* SDcard_h */