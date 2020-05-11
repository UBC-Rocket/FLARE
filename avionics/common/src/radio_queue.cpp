#include "radio_queue.h"

// recall:
// typedef SubPktPtr std::unique_ptr<std::vector<int>>;
void RadioQueue::push(SubPktPtr subpacket) {
    if (!subpacket) // check for null pointer
        return;
    if (subpacket->size() > RADIO_MAX_SUBPACKET_SIZE)
        return;

    m_byte_count += subpacket->size();
    m_subpacket_q.push_back(std::move(subpacket));

    while (m_byte_count > M_MAX_BYTES) {
        popFront();
    }
    return;
}

SubPktPtr RadioQueue::popFront() {
    SubPktPtr tmp = std::move(m_subpacket_q.front());
    m_subpacket_q.pop_front();
    m_byte_count -= tmp->size();
    return tmp;
}

uint8_t RadioQueue::fillPayload(uint8_t *payload) {
    if (m_byte_count <= RADIO_MAX_SUBPACKET_SIZE) {
        // Can fit everything in queue into payload.
        return dumpAllIntoPayload(payload);
    } else {
        return dispenseIntoPayload(payload);
    }
}

uint8_t RadioQueue::dumpAllIntoPayload(uint8_t *payload) {
    uint8_t start_byte_count = m_byte_count;
    for (SubPktPtr &i : m_subpacket_q) {
        payload = std::copy(i->begin(), i->end(), payload);
    }
    m_byte_count = 0;
    m_subpacket_q.clear();
    return start_byte_count;
}

uint8_t RadioQueue::dispenseIntoPayload(uint8_t *payload) {
    unsigned short used = 0;
    SubPktPtr i;
    while (true) {
        if (used + m_subpacket_q.front()->size() <= RADIO_MAX_SUBPACKET_SIZE) {
            i = popFront();
            payload = std::copy(i->begin(), i->end(), payload);
            used += m_subpacket_q.front()->size();
        } else {
            return used; // implicit break
        }
    }
}
