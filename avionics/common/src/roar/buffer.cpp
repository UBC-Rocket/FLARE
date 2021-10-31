#include <cassert>

#include "roar/buffer.hpp"

namespace roar {
/**
 * One extra space is needed for subpkts_ because you need one of them to
 * indicate the end of the last element.
 */
Buffer::Buffer(int pkt_size, int data_capacity, int subpkt_capacity)
    : data_(new uint8_t[data_capacity]),
      subpkts_(new Buffer::DataIt[subpkt_capacity + 1]), pkt_size_(pkt_size),
      data_cap_(data_capacity), subpkt_cap_(subpkt_capacity + 1),
      subpkt_begin_(0), subpkt_count_(0), pos_(0) {
    assert(subpkt_capacity > 0);
    // can't have more subpackets than bytes
    assert(data_capacity >= subpkt_capacity);
    assert(pkt_size > 0);

    subpkts_[0] = Buffer::DataIt{0};
}

int Buffer::usage() {
    // Check for zero prevents ambiguity from case where buffer is full
    if (subpkt_count_ == 0) {
        return 0;
    }

    DataIt end_of_last = deref(addIt(subpkt_begin_, subpkt_count_));
    return subIt(end_of_last, deref(subpkt_begin_));
}

/**
 * @brief Returns true if no data is in the buffer.
 */
bool Buffer::empty() { return subpkt_count_ == 0; }

/**
 * @brief Allocates space for a subpacket of specified `size`. Future calls
 * to write() will be done for the allocated subpacket.
 *
 * It is expected that the subpacket gets filled out immediately, before
 * space for the next subpacket is allocated.
 *
 * @param size Number of bytes to allocate
 * @return Number of subpackets that were dropped to make space
 */
int Buffer::allocSubpkt(int size) {
    // In addition to not making sense, a subpacket with size 0 screws with
    // computing data usage of the buffer
    assert(size > 0);
    assert(size <= pkt_size_);

    // Ensure there's space in the subpacket buffer
    int starting_subpkt_count = subpkt_count_;
    // recall: subpkt_cap_ = 1 + [# of subpackets that can be buffered]
    if (subpkt_count_ + 1 >= subpkt_cap_) {
        popSubpkt();
    }

    // Ensure there's space in the data buffer
    while (usage() + size > data_cap_) {
        popSubpkt();
    }
    int drop_count = starting_subpkt_count - subpkt_count_;

    // Get end of last subpacket already allocated
    const SubpktIt subpkts_end = addIt(subpkt_begin_, subpkt_count_);

    // previous usage of write() should've brought pos_ to the right place,
    // since otherwise the subpacket wouldn't be filled
    assert(pos_ == deref(subpkts_end));

    // but it's safest/more robust to enforce it anyways
    // Reset data pointer to newest subpacket (for use in write())
    pos_ = deref(subpkts_end);

    // Mark end of newly allocated subpacket
    deref(addIt(subpkts_end, 1)) = addIt(pos_, size);
    ++subpkt_count_;

    return drop_count;
}

void Buffer::write(uint8_t byte) {
    // make sure that we haven't overflowed the allocated space for a subpkt
    assert(pos_ != deref(addIt(subpkt_begin_, subpkt_count_)));

    deref(pos_) = byte;
    pos_ = addIt(pos_, 1);
}

/**
 * @brief Fills payload pointer as full as possible (i.e. at most
 * RADIO_MAX_SUBPACKET_SIZE bytes)
 * @param payload Pointer to payload array.
 * @return Number of bytes output to payload.
 */
int Buffer::fillPayload(uint8_t *payload) {
    assert(payload != nullptr);

    int used = 0;
    DataIt start = deref(subpkt_begin_);

    while (subpkt_count_ != 0 && used + firstSubpktSize() <= pkt_size_) {
        used += firstSubpktSize();
        popSubpkt(); // Popping subpacket doesn't overwrite the data
    }
    dumpContiguous(payload, start, used);
    return used;
}

/**
 * @brief Copies a continuous sequence of bytes from the buffer into the
 * destination.
 * @param dest Pointer to output buffer, must be at least `len` long
 * @param src Iterator to start of data to copy from circular buffer
 * @param len How many bytes to copy
 */
void Buffer::dumpContiguous(uint8_t *dest, DataIt src, int len) const {
    const int first_half = data_cap_ - src.index;
    if (len <= first_half) {
        // Stored data doesn't wrap
        std::memcpy(dest, &data_[src.index], len);
    } else {
        // data wraps
        std::memcpy(dest, &data_[src.index], first_half);
        std::memcpy(dest + first_half, data_.get(), len - first_half);
    }
}

/**
 * @brief Removes a packet from the start of the subpacket queue,
 * decrementing the counter of subpackets in the process.
 */
void Buffer::popSubpkt() {
    assert(subpkt_count_ > 0);

    subpkt_begin_ = addIt(subpkt_begin_, 1);
    subpkt_count_--;
}

/**
 * @brief Returns the size of the first subpacket, in bytes.
 */
int Buffer::firstSubpktSize() {
    return subIt(deref(addIt(subpkt_begin_, 1)), deref(subpkt_begin_));
}

/**
 * @brief Increments data iterator; takes care of wrapping
 */
Buffer::DataIt Buffer::addIt(Buffer::DataIt it, int add) {
    it.index += add;
    it.index %= data_cap_;
    return it;
}

/**
 * @brief Increments data iterator; takes care of wrapping
 */
Buffer::SubpktIt Buffer::addIt(Buffer::SubpktIt it, int add) {
    it.index += add;
    it.index %= subpkt_cap_;
    return it;
}

/**
 * @brief Difference between subpacket iterators. Returns size of array rather
 * than zero if iterators are equal.
 */
int Buffer::subIt(Buffer::SubpktIt a, Buffer::SubpktIt b) {
    // Modulo operator gives negative value if first operand is -ve
    if (a == b) {
        return subpkt_cap_;
    }
    return (a.index - b.index + subpkt_cap_) % subpkt_cap_;
}

/**
 * @brief Difference between data iterators. Returns size of array rather
 * than zero if iterators are equal.
 */
int Buffer::subIt(Buffer::DataIt a, Buffer::DataIt b) {
    // Modulo operator gives negative value if first operand is -ve
    if (a == b) {
        return data_cap_;
    }
    return (a.index - b.index + data_cap_) % data_cap_;
}

uint8_t &Buffer::deref(Buffer::DataIt it) {
    assert(it.index >= 0);
    assert(it.index < data_cap_);

    return data_[it.index];
}

Buffer::DataIt &Buffer::deref(Buffer::SubpktIt it) {
    assert(it.index >= 0);
    assert(it.index < subpkt_cap_);

    return subpkts_[it.index];
}

} // namespace roar
