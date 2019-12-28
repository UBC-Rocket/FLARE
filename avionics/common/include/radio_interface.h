#ifndef RADIO_INTERFACE_H_D7F3F9B440C74CEAB19571C6D763314B
#define RADIO_INTERFACE_H_D7F3F9B440C74CEAB19571C6D763314B

#include <memory> // for std::unique_ptr
#include <vector> // for std::vector

typedef std::unique_ptr<std::vector<int>> SubPktPtr;

class IRadioController {
public:
    /**
     * Add a subpacket to the queue to be sent. Note that this is a rather low-level utility; there should also be a helper method for any given subpacket that will build up the specific format this needs that you should use instead.
     * @param dat A SubPktPtr (refer to typedef) containing the data.
     */
    virtual void addSubpacket(SubPktPtr dat) = 0;

    /**
     * @brief Meat of the action - listens for any incoming packets, then transmits data and performs rocket actions as necessary.
     */
    virtual void listenAndAct() = 0;
};

#endif
