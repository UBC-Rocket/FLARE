#ifndef ROAR_BUFFER_HPP_E3C08775FC654E1784215F53447EBFC0
#define ROAR_BUFFER_HPP_E3C08775FC654E1784215F53447EBFC0

#include <cstdint>
#include <cstring> // memcpy
#include <memory>  // unique_ptr, used for fixed internal buffers on the heap
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
     * @param pkt_size Size (in bytes) of one subpacket; used in fillPayload.
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
     * @param size size of subpacket to be allocated
     * @return how many subpackets were dropped to make space.
     */
    int allocSubpkt(int size);

    /**
     * @brief Write data to the last allocated subpacket.
     * @param byte byte of data to be written
     */
    void write(uint8_t byte);

    /**
     * @brief Write data to the last allocated subpacket.
     * @param byte pointer to source 
     * @param len length of data we want to write in bytes
     */
    template <class T> void write(const T *byte, int len) {
        // TODO: figure out how to instantiate explicit versions in .cpp file
        const uint8_t *dat = reinterpret_cast<const uint8_t *>(byte);

        int space_remain = data_cap_ - pos_.index;
        if (len <= space_remain) {
            // fits within remaining space
            std::memcpy(&data_[pos_.index], dat, len);
        } else {
            // data wraps to start
            std::memcpy(&data_[pos_.index], dat, space_remain);
            std::memcpy(data_.get(), dat + space_remain, len - space_remain);
        }

        pos_ = addIt(pos_, len);
    }

    /**
     * @brief Fills payload pointer as full as possible (i.e. at most
     * pkt_size bytes, where pkt_size is what was passed into the constructor)
     * @param payload Pointer to payload array. Must be at least pkt_size long.
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
     * @param payload pointer to payload
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
     * @param it data iterator which we want to increment
     * @param add amount we want to increment data iterator by
     * @return the data iterator after incrementation 
     */
    DataIt addIt(DataIt it, int add);

    /**
     * @brief Increments data iterator; takes care of wrapping
     * @param it SubpktIt iterator which we want to increment
     * @param add amount we want to increment SubpktIt iterator by
     * @return the SubpktIt iterator after incrementation
     */
    SubpktIt addIt(SubpktIt it, int add);

    /**
     * @brief Difference between data iterator
     * @param a operand 1 
     * @param b operand 2
     * @return Difference between data iterators a and b
     */
    int subIt(DataIt a, DataIt b);

    /**
     * @brief Difference between subpacket iterator
     * @param a operand 1
     * @param b operand 2
     * @return Difference between subpacket iterators a and b
     */
    int subIt(SubpktIt a, SubpktIt b);

    /**
     * @brief Dereferences iterator
     * @param it data iterator we want to dereference
     * @return value of data iterator in data_ buffer
     */
    uint8_t &deref(DataIt it);

    /**
     * @brief Dereferences iterator
     * @param it SubpktIt iterator we want to derefence
     * @return value of SubpktIt iterator in subpkts_
     */
    DataIt &deref(SubpktIt it);

    // Types that store indices in a type-safe manner
    // Direct usage of the `index` member is discouraged; generally instead use
    // addIt(), subIt(), and deref() (which makes sure that the index wraps
    // properly)
    struct DataIt {

        /**
         * @brief Construct a new Data It object
         * @param i 
         */
        DataIt(int i) : index(i) {}

        /**
         * @brief Construct a new Data It object
         */
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
     * @return size of first subpacket, in bytes
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
