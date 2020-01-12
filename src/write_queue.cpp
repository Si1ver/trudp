/*
 * The MIT License
 *
 * Copyright 2016-2018 Kirill Scherba <kirill@scherba.ru>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \file   write_queue.c
 * \author Kirill Scherba <kirill@scherba.ru>
 *
 * Created on June 15, 2016, 12:56 AM
 */

#include <stdlib.h>
#include <string.h>

#include <queue>

#include "write_queue.h"

/**
 * Create new Write queue
 *
 * @return Pointer to trudpWriteQueue
 */

trudpWriteQueue* trudpWriteQueueNew() {
    return new trudpWriteQueue;
}
/**
 * Destroy Write queue
 *
 * @param wq Pointer to trudpWriteQueue
 */

void trudpWriteQueueDestroy(trudpWriteQueue* wq) {
    if (wq != nullptr) {
        delete wq;
    }
}
/**
 * Remove all elements from Write queue
 *
 * @param wq Pointer to trudpWriteQueue
 * @return Zero at success
 */

void trudpWriteQueueFree(trudpWriteQueue* wq) {
    if (wq == nullptr) {
        return;
    }

    std::queue<trudpWriteQueueData> new_q;
    std::swap(wq->q, new_q);
}
/**
 * Get number of elements in Write queue
 *
 * @param wq
 *
 * @return Number of elements in Write queue
 */

size_t trudpWriteQueueSize(trudpWriteQueue* wq) {
    if (wq == nullptr) {
        return 0;
    }

    return wq->q.size();
}
/**
 * Get pointer to first element data
 *
 * @param wq Pointer to trudpWriteQueue
 *
 * @return Pointer to trudpWriteQueueData data or NULL
 */

trudpWriteQueueData* trudpWriteQueueGetFirst(trudpWriteQueue* wq) {
    if (wq == nullptr) {
        return nullptr;
    }

    if (wq->q.empty()) {
        return nullptr;
    }

    return &wq->q.front();
}
/**
 * Remove first element from Write queue
 *
 * @param wq Pointer to trudpWriteQueue
 *
 * @return Zero at success
 */

void trudpWriteQueueDeleteFirst(trudpWriteQueue* wq) {
    wq->q.pop();
}

/**
 * Add packet to Write queue
 *
 * @param wq Pointer to trudpWriteQueue
 * @param packet Pointer to Packet to add to queue
 * @param packet_ptr Pointer to Packet to add to queue
 * @param packet_length Packet length
 *
 * @return Pointer to added trudpWriteQueueData
 */
trudpWriteQueueData* trudpWriteQueueAdd(
    trudpWriteQueue* wq, uint8_t* packet, size_t packet_length) {
    trudpWriteQueueData new_item;
    new_item.packet_length = packet_length;
    new_item.packet.insert(new_item.packet.end(), packet, packet + packet_length);

    wq->q.emplace(std::move(new_item));

    return &wq->q.back();
}
