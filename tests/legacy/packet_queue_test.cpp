/*
 * File:   header_t.c
 * Author: Kirill Scherba <kirill@scherba.ru>
 *
 * Created on May 30, 2016, 7:41:24 PM
 */

#include <cstddef>
#include <cstdint>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "packet.h"
#include "packet_queue.h"

/**
 * Timed queue test
 */
TEST(LegacyTest, PacketQueueOperations) {
    uint32_t id = 0;
    trudpPacketQueueData *tqd;

    // Create timed queue
    trudpPacketQueue *tq = trudpPacketQueueNew();

    // Create 1 DATA packet
    const char *data = "Header with Hello!";
    size_t packetLength, data_length = strlen(data) + 1;
    uint32_t packet_id = ++id;
    trudpPacket* packetDATA = trudpPacketDATAcreateNew(packet_id, 0, (void*)data, data_length, &packetLength);
    EXPECT_EQ(trudpPacketGetId(packetDATA), packet_id);

    // Create 2 DATA packet
    const char *data2 = "Header with Hello 2!";
    size_t packetLength2, data_length2 = strlen(data2) + 1;
    uint32_t packet_id2 = ++id;
    trudpPacket* packetDATA2 =
        trudpPacketDATAcreateNew(packet_id2, 0, (void*)data2, data_length2, &packetLength2);
    EXPECT_EQ(trudpPacketGetId(packetDATA2), packet_id2);

    // Add 1 DATA packet to timed queue
    tqd = trudpPacketQueueAdd(tq, (uint8_t*)packetDATA, packetLength, teoGetTimestampFull() + 10000);
    trudpPacket* tq_packet = trudpPacketQueueDataGetPacket(tqd);
    EXPECT_EQ(trudpPacketGetId(tq_packet), packet_id);

    // Add 2 DATA packet to timed queue
    tqd = trudpPacketQueueAdd(tq, (uint8_t*)packetDATA2, packetLength2, teoGetTimestampFull() + 10000);
    tq_packet = trudpPacketQueueDataGetPacket(tqd);
    EXPECT_EQ(trudpPacketGetId(tq_packet), packet_id2);

    // Find DATA packet by Id
    EXPECT_EQ(tqd, trudpPacketQueueFindById(tq, packet_id2));

    // Free timed queue
    trudpPacketQueueFree(tq);
    EXPECT_EQ(trudpPacketQueueSize(tq), 0);

    // Free DATA packet
    trudpPacketCreatedFree(packetDATA);

    // Destroy timed queue
    trudpPacketQueueDestroy(tq);
}
