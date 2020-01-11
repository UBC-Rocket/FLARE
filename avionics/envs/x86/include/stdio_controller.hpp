#ifndef COUT_CONTROLLER_H_28309666D6B04A8EA72B8C7C06476AE5
#define COUT_CONTROLLER_H_28309666D6B04A8EA72B8C7C06476AE5

#include <iostream>
#include <thread> //for non-blocking input
#include <unordered_map> //for hash map
#include <vector>

class StdIoController {
public:
    /**
     * @brief Attempts to extract a single character from the specified buffer Id.
     * @param id ID being used (see documentation on stdio multiplexing spec: http://confluence.ubcrocket.com/display/AV/Specs)
     * @param c If read succeeded, the result is written to c.
     * @return True if read succeeded, false if EOF.
     */
    bool get(uint8_t id, unsigned char &c) {
        m_mutexes[id].lock();
        bool tmp = bool(m_istreams[id].get(c));
        m_mutexes[id].unlock();
        return tmp;
    }

    /**
     * @brief Basic level utility to put a packet into std::cout.
     * @param id ID being used
     * @param c Pointer to array of data to be sent
     * @param length Length of the data to be sent
     */
    void putPacket(uint8_t const id, unsigned char const * const c, uint16_t const length){
        m_cout.lock();
        //TODO - check the success of std::cout.put and other unformatted output, and possibly do something about it
        std::cout.put(id); //ID
        std::cout.put(static_cast<char>(length >> 8)); //Length, bigendian
        std::cout.put(static_cast<char>(length & 0xFF));
        std::cout.write(c, length); //Data
        m_cout.unlock();
    }

private:
    typedef uint8_t Id;

    std::unordered_map<Id, std::mutex> m_mutexes;
    std::unordered_map<Id, std::stringstream> m_istreams; //Note - for input only

    // I got tired of thinking about the best way to pre-compose strings so instead I'm just going to lock std::cout
    std::mutex m_cout;


    std::thread m_input; //run infinite inputLoop()

    char getCinForce(){
        char c;
        while(!std::cin.get(id));
        return c;
    }


    void inputLoop(){
        Id id;
        uint16_t length;

        for(;;){
            if(std::cin.get(id)){
                length = getCinForce();
                length <<= 8;
                length |= getCinForce();

                auto buf = std::vector<char>();
                buf.reserve(length);
                for(int i = 0; i < length; i++) {
                    buf.push_back(getCinForce());
                }

                m_mutexes[id].lock();
                for(auto j : buf) {
                    m_istreams.put(j);
                }
                m_mutexes[id].unlock();
            }
        }
    }
}

#endif
