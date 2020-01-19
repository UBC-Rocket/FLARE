#ifndef COUT_CONTROLLER_H_28309666D6B04A8EA72B8C7C06476AE5
#define COUT_CONTROLLER_H_28309666D6B04A8EA72B8C7C06476AE5

#include <iostream> //for std::cin/cout
#include <queue> //for queue
#include <thread> //for non-blocking input
#include <mutex>
#include <unordered_map> //for hash map
#include <vector>

class StdIoController {
public:
    StdIoController() : m_input(&StdIoController::inputLoop, this) {}
    /**
     * @brief Attempts to extract a single character from the specified buffer Id.
     * @param id ID being used (see documentation on stdio multiplexing spec: http://confluence.ubcrocket.com/display/AV/Specs)
     * @param c If read succeeded, the result is written to c.
     * @return True if read succeeded, false if EOF.
     */
    bool get(uint8_t id, uint8_t &c) {
        m_mutexes[id].lock();
        if (m_istreams[id].size() == 0){
            return false;
        }
        c = m_istreams[id].front();
        m_istreams[id].pop();
        m_mutexes[id].unlock();
        return true;
    }

    /**
     * @brief Basic level utility to put a packet into std::cout.
     * @param id ID being used
     * @param c Pointer to array of data to be sent
     * @param length Length of the data to be sent
     */
    void putPacket(uint8_t const id, char const * const c, uint16_t const length){
        m_cout.lock();
        //TODO - check the success of std::cout.put and other unformatted output, and possibly do something about it
        std::cout.put(id); //ID
        std::cout.put(static_cast<char>(length >> 8)); //Length, bigendian
        std::cout.put(static_cast<char>(length & 0xFF));
        std::cout.write(c, length); //Data
        m_cout.unlock();
    }

    /**
     * @brief Basic level utility to put a packet into std::cout.
     * @param id ID being used
     * @param c Pointer to array of data to be sent
     * @param length Length of the data to be sent
     */
    void putPacket(uint8_t const id, uint8_t const *c, uint16_t const length){
        putPacket(id, reinterpret_cast<const char*>(c), length);
        //Shoudn't need to worry that std::cin et al thinks its pointing to a char - the bits should come out as defined by uint8_t, which would be expected. Char is defined to be a byte, so this is fine as long as a byte is 8 bits (no seriously there are some machines that have bytes with more than 8 bits.)
        //TODO - maybe less sloppily deal with this problem without using reinterpret_cast? Although we're low enough that reinterpret is close to being allowed
    }

private:
    typedef char Id;

    std::unordered_map<Id, std::mutex> m_mutexes;
    std::unordered_map<Id, std::queue<uint8_t>> m_istreams; //Note - for input only

    // I got tired of thinking about the best way to pre-compose strings so instead I'm just going to lock std::cout
    std::mutex m_cout;


    std::thread m_input; //run infinite inputLoop()

    char getCinForce(){
        //busy loops until we get a character
        char c;
        while(!std::cin.get(c));
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
                    m_istreams[id].push(j);
                }
                m_mutexes[id].unlock();
            }
        }
    }
};

#endif
