#ifndef ROAR_BUFFER_HPP_E3C08775FC654E1784215F53447EBFC0
#define ROAR_BUFFER_HPP_E3C08775FC654E1784215F53447EBFC0

#include <cstdint>
#include <memory>

namespace roar {

/**
 * Subpackets are written one at a time. `allocSubpkt` allocates space for a
 * subpacket in the internal buffer. Older subpackets will be dropped fully if
 * there is not enough space.
 *
 * When allocating space for a subpacket, it should be immediately filled. Once
 * `allocSubpkt()` or `fillPayload()` is called, the previous subpacket is
 * "committed"; i.e. it is fixed and is no longer editable.
 *
 * Usage:
 *
 * allocSubpkt()
 * write()
 * write() // Can call multiple times
 * fillPayload() // "commits" the last subpacket
 */
class Buffer {
  public:
    /**
     * @param data_capacity Space (in bytes) for the underlying buffer.
     * @param subpkt_capacity Maximum number of subpackets to keep track
     * of. Should be less than data_capacity.
     */
    Buffer(int pkt_size, int data_capacity, int subpkt_capacity);

    /**
     * @brief Allocates space for a subpacket of specified `size`. Future calls
     * to write() will be done for the allocated subpacket.
     *
     * It is expected that the subpacket gets filled out immediately, before
     * space for the next subpacket is allocated.
     *
     * Returns how many subpackets were dropped to make space.
     */
    int allocSubpkt(int size);

    /**
     * @brief Write data to the last allocated subpacket.
     */
    void write(uint8_t byte);

    /**
     * @brief Write data to the last allocated subpacket.
     */
    void write(uint8_t *byte, int len);

    /**
     * @brief Fills payload pointer as full as possible (i.e. at most
     * RADIO_MAX_PACKET_SIZE bytes)
     * @param payload Pointer to payload array.
     * @return Size of payload data filled in.
     */
    int fillPayload(uint8_t *payload);

    /**
     * @brief Returns how many bytes are in use in the data buffer.
     */
    int usage();

    /**
     * @brief Returns true if no data is in the buffer.
     */
    bool empty();

  private:
    class DataIt;
    class SubpktIt;

    /**
     * @brief Dumps contiguous section of data circular buffer into payload. Can
     * be used to dump all of queue data into payload. Does not modify any class
     * variables.
     * @param dest Buffer to output
     * @param src Starting position in circular buffer
     * @param len Number of bytes to copy
     */
    void dumpContiguous(uint8_t *dest, DataIt src, int len) const;

    /**
     * @brief Dispenses at most pkt_size_ bytes worth of
     * subpackets into payload.
     * @return Size of payload data filled in.
     */
    int dispenseIntoPayload(uint8_t *payload);

    /**
     * @brief Removes a packet from the start of the subpacket queue,
     * decrementing the counter of subpackets in the process.
     */
    void popSubpkt();

    // Iterator operations are implemented as normal functions instead of
    // operator overloading since operator overloading would require storing a
    // pointer to the Buffer inside the iterator structs
    /**
     * @brief Increments data iterator; takes care of wrapping
     */
    DataIt addIt(DataIt it, int add);

    /**
     * @brief Increments data iterator; takes care of wrapping
     */
    SubpktIt addIt(SubpktIt it, int add);

    /**
     * @brief Difference between subpacket iterator
     */
    int subIt(DataIt a, DataIt b);

    /**
     * @brief Difference between subpacket iterator
     */
    int subIt(SubpktIt a, SubpktIt b);

    /**
     * @brief Dereferences iterator
     */
    uint8_t &deref(DataIt it);

    /**
     * @brief Dereferences iterator
     */
    DataIt &deref(SubpktIt it);

    // Types that store indices in a type-safe manner
    // Direct usage of the `index` member is discouraged; generally instead use
    // addIt(), subIt(), and deref() (which makes sure that the index wraps
    // properly)
    struct DataIt {
        DataIt(int i) : index(i) {}
        DataIt() : DataIt(0) {}

        bool operator==(const DataIt &other) const {
            return other.index == index;
        };

        bool operator!=(const DataIt &other) const {
            return other.index != index;
        };

        int index;
    };

    struct SubpktIt {
        SubpktIt(int i) : index(i) {}
        SubpktIt() : SubpktIt(0) {}

        bool operator==(const SubpktIt &other) const {
            return other.index == index;
        };

        bool operator!=(const SubpktIt &other) const {
            return other.index != index;
        };

        int index;
    };

    /**
     * @brief Returns the size of the first subpacket, in bytes.
     */
    int firstSubpktSize();

    // Circular buffer holding the actual bytes
    std::unique_ptr<uint8_t[]> data_;

    // Circular buffer holding subpacket info
    std::unique_ptr<DataIt[]> subpkts_;

    /*
    First value of subpkts_ points to first byte of the first subpacket.
    Each of the following values points to one past the last byte of the
    previous subpacket (which is equivalently the first byte of the
    current subpacket, except for the last value of subpkts_). Exception
    is when there are no subpackets, in which case there is exactly one
    defined value in subpkts_ (it doesn't really matter where it points
    in this situation - there's no data in the buffer)
    */

    // Maximum size of a packet
    const int pkt_size_;

    // capacities of buffers
    const int data_cap_;
    // one larger than constructor parameter - see .cpp file
    const int subpkt_cap_;

    SubpktIt subpkt_begin_; // Starting index of used subpackets
    int subpkt_count_;      // number subpackets used

    // Position of current data pointer
    DataIt pos_;
};

/*  Many parts of the XBee library use uint8 for length (e.g. getFrameData).
    Even though the HW likely supports more, this is the theoretical max for
    this XBee library implementation. If we deem this was an implementation
    error, we could PR a fix to the library and then increase this */

} // namespace roar
#endif
