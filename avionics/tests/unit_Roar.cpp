#include "gtest/gtest.h"

#include <vector>

#include "roar/buffer.hpp"

/**
 * Test happy path
 */
TEST(RoarBuffer, HappyPath) {
    constexpr int kPktSize = 32;
    constexpr int kBuffSize = 64;
    roar::Buffer buff(kPktSize, kBuffSize, 16);

    EXPECT_EQ(buff.usage(), 0);
    EXPECT_EQ(buff.allocSubpkt(16), 0);
    EXPECT_EQ(buff.usage(), 16);

    for (int i = 0; i < 8; ++i) {
        buff.write(i);
    }

    { // scope to reuse `dat` variable
        std::vector<uint8_t> dat{0, 1, 2, 3, 4, 5, 6, 7};
        buff.write(dat.data(), 8);
    }

    { // scope to reuse `dat` variable
        EXPECT_EQ(buff.allocSubpkt(8), 0);
        std::vector<uint8_t> dat;
        for (int i = 8; i < 16; ++i) {
            dat.reserve(8);
            dat.push_back(i);
        }
        buff.write(dat.data(), 8);
    }
    EXPECT_EQ(buff.usage(), 24);

    const std::vector<uint8_t> payloadDefault(64, 255);
    // Padded with 255 to detect out-of-bounds writes
    std::vector<uint8_t> payload(payloadDefault);

    EXPECT_EQ(buff.fillPayload(payload.data() + 16), 24);
    EXPECT_EQ(buff.usage(), 0);

    for (int i = 0; i < 16; ++i) {
        EXPECT_EQ(payload[i], 255);
    }
    for (int i = 16; i < 32; ++i) {
        EXPECT_EQ(payload[i], i % 8);
    }
    for (int i = 32; i < 40; ++i) {
        EXPECT_EQ(payload[i], i % 8 + 8);
    }
    for (int i = 40; i < 64; ++i) {
        EXPECT_EQ(payload[i], 255);
    }

    // reset payload
    payload = payloadDefault;

    for (int i = 16; i < 16 + kBuffSize / 8; ++i) {
        EXPECT_EQ(buff.allocSubpkt(8), 0);
        for (int j = 0; j < 8; ++j) {
            buff.write(i);
        }
    }

    EXPECT_EQ(buff.fillPayload(payload.data() + 16), kPktSize);

    for (int i = 0; i < 16; ++i) {
        EXPECT_EQ(payload[i], 255);
    }
    for (int i = 16; i < 16 + kPktSize / 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            EXPECT_EQ(payload[16 + (i - 16) * 8 + j], i);
        }
    }
    for (int i = 56; i < 64; ++i) {
        EXPECT_EQ(payload[i], 255);
    }
}

void fillSubpkt(roar::Buffer &buff, int val, int len) {
    for (; len > 0; --len) {
        buff.write(val);
    }
}

/**
 * Test behaviour when data buffer is overflowed, so subpackets need to be
 * dropped
 */
TEST(RoarBuffer, OverflowData) {
    constexpr int kPktSize = 50;
    constexpr int kBuffSize = 128;
    roar::Buffer buff(kPktSize, kBuffSize, kBuffSize);

    constexpr int kSubpktSize0 = 16;

    // Write to fill internal buffer, but don't overflow
    for (int i = 0; i < kBuffSize / kSubpktSize0; ++i) {
        EXPECT_EQ(buff.allocSubpkt(kSubpktSize0), 0);
        fillSubpkt(buff, 0, kSubpktSize0);
    }

    // Should throw out subpkt of size 16
    EXPECT_EQ(buff.allocSubpkt(1), 1);
    buff.write(10); // fill subpkt
    EXPECT_EQ(buff.allocSubpkt(1), 0);
    buff.write(11);                     // fill subpkt
    EXPECT_EQ(buff.allocSubpkt(14), 0); // buffer now full
    fillSubpkt(buff, 12, 14);

    EXPECT_EQ(buff.allocSubpkt(8), 1);
    fillSubpkt(buff, 13, 8);
    EXPECT_EQ(buff.allocSubpkt(9), 1);
    fillSubpkt(buff, 14, 9);
    EXPECT_EQ(buff.allocSubpkt(30), 1);
    fillSubpkt(buff, 15, 30);
    EXPECT_EQ(buff.allocSubpkt(18), 2);
    fillSubpkt(buff, 16, 18);
    EXPECT_EQ(buff.allocSubpkt(31), 1);
    fillSubpkt(buff, 17, 31);

    EXPECT_EQ(buff.allocSubpkt(19), 4);
    fillSubpkt(buff, 18, 19);

    std::vector<uint8_t> payload(kPktSize + 1, 255);
    EXPECT_EQ(buff.fillPayload(payload.data()), 47);
    auto it = payload.begin();
    for (int i = 0; i < 8; ++i, ++it) {
        EXPECT_EQ(*it, 13);
    }
    for (int i = 0; i < 9; ++i, ++it) {
        EXPECT_EQ(*it, 14);
    }
    for (int i = 0; i < 30; ++i, ++it) {
        EXPECT_EQ(*it, 15);
    }
    EXPECT_EQ(*it, 255); // nothing left

    payload = std::vector<uint8_t>(kPktSize + 1, 255);
    EXPECT_EQ(buff.fillPayload(payload.data()), 49);
    it = payload.begin();
    for (int i = 0; i < 18; ++i, ++it) {
        EXPECT_EQ(*it, 16);
    }
    for (int i = 0; i < 31; ++i, ++it) {
        EXPECT_EQ(*it, 17);
    }
    EXPECT_EQ(*it, 255);
}

/**
 * Test behaviour when subpacket array is overflowed, i.e. too many (small)
 * subpackets, so they need to be dropped
 */
TEST(RoarBuffer, OverflowSubpackets) {
    constexpr int kPktSize = 50;
    constexpr int kBuffSize = 200;
    roar::Buffer buff(kPktSize, kBuffSize, 10);

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(buff.allocSubpkt(1), 0);
        buff.write(i);
    }
    // Now subpacket limit has been reached

    EXPECT_EQ(buff.allocSubpkt(1), 1);
    buff.write(10);
    EXPECT_EQ(buff.allocSubpkt(1), 1);
    buff.write(11);

    std::vector<uint8_t> payload(kPktSize + 1, 255);
    EXPECT_EQ(buff.fillPayload(payload.data()), 10);

    auto it = payload.begin();
    for (int i = 0; i < 10; ++i, ++it) {
        EXPECT_EQ(*it, i + 2);
    }
    EXPECT_EQ(*it, 255);
    EXPECT_EQ(buff.usage(), 0);
}

/**
 * Test behaviour when both subpacket array and data array are overflowed
 */
TEST(RoarBuffer, OverflowBoth) {
    constexpr int kPktSize = 50;
    constexpr int kBuffSize = 180;
    roar::Buffer buff(kPktSize, kBuffSize, 6);

    for (int i = 0; i < 6; ++i) {
        buff.allocSubpkt(30);
        fillSubpkt(buff, i, 30);
    }
    // Internal buffer is now full
    EXPECT_EQ(buff.allocSubpkt(50), 2);
    fillSubpkt(buff, 100, 50);

    // Four packets from original loop are still here
    for (int i = 0; i < 4; ++i) {
        std::vector<uint8_t> payload(kPktSize + 1, 255);
        EXPECT_EQ(buff.fillPayload(payload.data()), 30);
        for (int j = 0; j < 30; ++j) {
            EXPECT_EQ(payload[j], i + 2);
        }
        EXPECT_EQ(payload[30], 255);
    }

    std::vector<uint8_t> payload(kPktSize + 1, 255);
    EXPECT_EQ(buff.fillPayload(payload.data()), 50);
    for (int i = 0; i < 50; ++i) {
        for (int j = 0; j < 50; ++j) {
            EXPECT_EQ(payload[j], 100);
        }
    }
    EXPECT_EQ(payload[50], 255);
}

/**
 * Test correct behaviour when filling payloads but the buffer is empty
 */
TEST(RoarBuffer, Empty) {
    roar::Buffer buff(10, 10, 10);
    std::vector<uint8_t> payload(10, 255);
    EXPECT_EQ(buff.fillPayload(payload.data()), 0);
    EXPECT_EQ(payload[0], 255);

    // Fill w/out overflow and discard
    buff.allocSubpkt(5);
    fillSubpkt(buff, 0, 5);
    buff.allocSubpkt(5);
    fillSubpkt(buff, 1, 5);
    std::vector<uint8_t> discard(10);
    EXPECT_EQ(buff.fillPayload(discard.data()), 10);

    // empty again
    EXPECT_EQ(buff.fillPayload(payload.data()), 0);
    EXPECT_EQ(payload[0], 255);

    // Fill w/ overflow and discard
    buff.allocSubpkt(5);
    fillSubpkt(buff, 10, 5);
    buff.allocSubpkt(5);
    fillSubpkt(buff, 11, 5);
    buff.allocSubpkt(5);
    fillSubpkt(buff, 12, 5);
    EXPECT_EQ(buff.fillPayload(discard.data()), 10);

    // empty again
    EXPECT_EQ(buff.fillPayload(payload.data()), 0);
    EXPECT_EQ(payload[0], 255);
}

/**
 * Test writing enough to cause the circular buffer to wrap
 */
TEST(RoarBuffer, WrapWrite0) {
    roar::Buffer buff(10, 10, 10);
    buff.allocSubpkt(8);
    fillSubpkt(buff, 10, 8);

    EXPECT_EQ(buff.allocSubpkt(4), 1);
    for (int i = 0; i < 4; ++i) {
        buff.write(i);
    }

    std::vector<uint8_t> payload(10, 255);
    EXPECT_EQ(buff.fillPayload(payload.data()), 4);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(payload[i], i);
    }
}

TEST(RoarBuffer, WrapWrite1) {
    roar::Buffer buff(10, 10, 10);
    buff.allocSubpkt(8);
    fillSubpkt(buff, 10, 8);

    EXPECT_EQ(buff.allocSubpkt(4), 1);
    std::vector<uint8_t> data{0, 1, 2, 3};
    buff.write(data.data(), 2);
    buff.write(data.data() + 2, 2);

    std::vector<uint8_t> payload(10, 255);
    EXPECT_EQ(buff.fillPayload(payload.data()), 4);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(payload[i], i);
    }
}

TEST(RoarBuffer, WrapWrite2) {
    roar::Buffer buff(10, 10, 10);
    buff.allocSubpkt(8);
    fillSubpkt(buff, 10, 8);

    EXPECT_EQ(buff.allocSubpkt(4), 1);
    std::vector<uint8_t> data{0, 1, 2, 3};
    buff.write(data.data(), 4);

    std::vector<uint8_t> payload(10, 255);
    EXPECT_EQ(buff.fillPayload(payload.data()), 4);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(payload[i], i);
    }
}

TEST(RoarBuffer, WrapWrite3) {
    roar::Buffer buff(10, 10, 10);
    buff.allocSubpkt(8);
    fillSubpkt(buff, 10, 8);

    EXPECT_EQ(buff.allocSubpkt(4), 1);
    std::vector<uint8_t> data{0, 1, 2, 3};
    buff.write(data.data(), 1);
    buff.write(data.data() + 1, 1);
    buff.write(data.data() + 1, 0);
    buff.write(data.data() + 2, 1);
    buff.write(data.data() + 2, 0);
    buff.write(data.data() + 3, 1);

    std::vector<uint8_t> payload(10, 255);
    EXPECT_EQ(buff.fillPayload(payload.data()), 4);
    for (int i = 0; i < 4; ++i) {
        EXPECT_EQ(payload[i], i);
    }
}
