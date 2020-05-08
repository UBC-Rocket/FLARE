#ifndef COUT_CONTROLLER_H_28309666D6B04A8EA72B8C7C06476AE5
#define COUT_CONTROLLER_H_28309666D6B04A8EA72B8C7C06476AE5

#include <iostream> //for std::cin/cout
#include <queue> //for queue
#include <thread> //for non-blocking input
#include <mutex>
#include <unordered_map> //for hash map
#include <vector>
#include <cstring> //for memmove

class StdIoController {
public:
    StdIoController() : m_input(&StdIoController::inputLoop, this) {
        putConfigPacket();
    }

    /**
     * @brief Attempts to extract a single character from the specified buffer Id.
     * @param id ID being used (see documentation on stdio multiplexing spec: http://confluence.ubcrocket.com/display/AV/Specs)
     * @param c If read succeeded, the result is written to c.
     * @return True if read succeeded, false if EOF.
     */
    bool get(uint8_t id, uint8_t &c) {
        const std::lock_guard<std::mutex> lock(m_mutexes[id]);

        if (m_istreams[id].size() == 0){
            return false;
        }
        c = m_istreams[id].front();
        m_istreams[id].pop();
        return true;
    }

    /**
     * @brief Basic level utility to put a packet into std::cout.
     * @param id ID being used
     * @param c Pointer to array of data to be sent
     * @param length Length of the data to be sent
     */
    static void putPacket(uint8_t const id, char const * const c, uint16_t const length){
        const std::lock_guard<std::mutex> lock(s_cout);
        //TODO - check the success of std::cout.put and other unformatted output, and possibly do something about it
        std::cout.put(id); //ID
        std::cout.put(static_cast<char>(length >> 8)); //Length, bigendian
        std::cout.put(static_cast<char>(length & 0xFF));
        std::cout.write(c, length); //Data
    }

    /**
     * @brief Basic level utility to put a packet into std::cout.
     * @param id ID being used
     * @param c Pointer to array of data to be sent
     * @param length Length of the data to be sent
     */
    static void putPacket(uint8_t const id, uint8_t const *c, uint16_t const length){
        putPacket(id, reinterpret_cast<const char*>(c), length);
        //Shoudn't need to worry that std::cin et al thinks its pointing to a char - the bits should come out as defined by uint8_t, which would be expected. Char is defined to be a byte, so this is fine as long as a byte is 8 bits (no seriously there are some machines that have bytes with more than 8 bits.)
        //TODO - maybe less sloppily deal with this problem without using reinterpret_cast? Although we're low enough that reinterpret is close to being allowed
    }

    /**
     * @brief Helper function for configuration packet.
     */
    static void putConfigPacket(){
        uint8_t id = 0x01;
        uint32_t int_test = 0x04030201;
        float float_test = -2.0; //0xC000 0000;

        char buf[8];
        std::memmove(buf, &int_test, 4);
        std::memmove(buf + 4, &float_test, 4);
        putPacket(id, buf, 8);
    }

private:
    typedef char Id;

    std::unordered_map<Id, std::mutex> m_mutexes;
    std::unordered_map<Id, std::queue<uint8_t>> m_istreams; //Note - for input only

    // I got tired of thinking about the best way to pre-compose strings so instead I'm just going to lock std::cout
    static std::mutex s_cout;

    std::thread m_input; //run infinite inputLoop()

    char getCinForce(){
        char c[2];
        std::cin.getline(c, 2, '\0');
        //getline always appends '\0' to the end, so if the character actually is '\0' then it will be discarded, but '\0' will still be returned
        return c[0];
    }


    void inputLoop(){
        Id id;
        uint16_t length;

        for(;;){
            id = getCinForce();
            length = getCinForce();
            length <<= 8;
            length |= getCinForce();

            auto buf = std::vector<char>();
            buf.reserve(length);
            for(int i = 0; i < length; i++) {
                buf.push_back(getCinForce());
            }

            { //scope for lock-guard
            const std::lock_guard<std::mutex> lock(m_mutexes[id]);
            for(auto j : buf) {
                m_istreams[id].push(j);
            } // unlock mutex
            
            }
        }
    }
};

#endif
