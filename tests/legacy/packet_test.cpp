/*
 * File:   header_t.c
 * Author: Kirill Scherba <kirill@scherba.ru>
 *
 * Created on May 30, 2016, 7:41:24 PM
 */

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "packet.h"

TEST(LegacyTest, PacketCreation) {
    uint32_t id = 0;

    // Create & check DATA packet
    std::string data_string = "Header with Hello!";
    size_t packetLength;
    size_t data_length = data_string.length() + 1;
    uint8_t* data_pointer = (uint8_t*)data_string.c_str();
    uint32_t packetDATAid = ++id;
    trudpPacket* packetDATA = trudpPacketDATAcreateNew(packetDATAid, 0, data_pointer, data_length, &packetLength);
    EXPECT_THAT(trudpPacketCheck((uint8_t*)packetDATA, packetLength), testing::NotNull());

    // Check getter functions
    EXPECT_EQ(trudpPacketGetId(packetDATA), packetDATAid);
    EXPECT_EQ(trudpPacketGetDataLength(packetDATA), data_length);
    EXPECT_EQ(memcmp(trudpPacketGetData(packetDATA), data_pointer, data_length), 0);
    EXPECT_EQ(trudpPacketGetType(packetDATA), trudppp::PacketType::Data);
    EXPECT_THAT(trudpPacketGetTimestamp(packetDATA), testing::Le(trudpGetTimestamp()));

    // Create & check ACK packet
    trudpPacket* packetACK = trudpPacketACKcreateNew(packetDATA);
    EXPECT_THAT(trudpPacketCheck((uint8_t*)packetACK, trudpPacketACKlength()), testing::NotNull());
    EXPECT_EQ(trudpPacketGetType(packetACK), trudppp::PacketType::Ack);

    // Create & check RESET packet
    trudpPacket* packetRESET = trudpPacketRESETcreateNew(++id,0);
    EXPECT_THAT(trudpPacketCheck((uint8_t*)packetRESET, trudpPacketRESETlength()), testing::NotNull());
    EXPECT_EQ(trudpPacketGetType(packetRESET), trudppp::PacketType::Reset);

    // Free packets
    trudpPacketCreatedFree(packetRESET);
    trudpPacketCreatedFree(packetACK);
    trudpPacketCreatedFree(packetDATA);
}
