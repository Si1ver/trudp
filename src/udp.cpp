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
 */

/**
 * UDP client server helper module
 */

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>

#include "udp.h"
#include "trudp_utils.h"
#include "trudp_options.h"

#include "teobase/logging.h"

// Global teocli options
extern bool trudpOpt_DBG_sendto;

// UDP / UDT functions
#define _trudpUdpSocket(domain, type, protocol) socket(domain, type, protocol)
#define _trudpUdpBind(fd, addr, addr_len) bind(fd, addr, addr_len)
#define NUMBER_OF_TRY_PORTS 1000

// Local functions
static void _trudpUdpHostToIp(struct sockaddr_in* remaddr, const char* server);
static void _trudpUdpSetNonblock(int fd);

/**
 * Set socket or FD to non blocking mode
 *
 * @param fd
 */
static void _trudpUdpSetNonblock(int fd) {
#if defined(HAVE_MINGW) || defined(_WIN32)
    //-------------------------
    // Set the socket I/O mode: In this case FIONBIO
    // enables or disables the blocking mode for the
    // socket based on the numerical value of iMode.
    // If iMode = 0, blocking is enabled;
    // If iMode != 0, non-blocking mode is enabled.

    int iResult;
    u_long iMode = 1;

    iResult = ioctlsocket(fd, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
        printf("ioctlsocket failed with error: %ld\n", iResult);
#else
    int flags;

    flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
}

/**
 * Convert host name to IP
 *
 * @param remaddr
 * @param server
 */
static void _trudpUdpHostToIp(struct sockaddr_in* remaddr, const char* server) {
    int result = inet_pton(AF_INET, server, &remaddr->sin_addr);

    if (result != -1) {
        /* When passing the host name of the server as a */
        /* parameter to this program, use the gethostbyname() */
        /* function to retrieve the address of the host server. */
        /***************************************************/
        /* get host address */
        struct hostent* hostp = gethostbyname(server);
        if (hostp == NULL) {
            // ...
        } else {
            memcpy(&remaddr->sin_addr, hostp->h_addr_list[0], sizeof(remaddr->sin_addr));
        }
    }
}

/**
 * Make address from the IPv4 numbers-and-dots notation and integer port number
 * into binary form
 *
 * @param addr
 * @param port
 * @param remaddr
 * @param addr_length
 * @return
 */
int trudpUdpMakeAddr(const char* addr, int port, __SOCKADDR_ARG remaddr, socklen_t* addr_length) {
    if (*addr_length < sizeof(struct sockaddr_in))
        return -3;

    *addr_length = sizeof(struct sockaddr_in); // length of addresses
    memset((void*)remaddr, 0, *addr_length);
    ((struct sockaddr_in*)remaddr)->sin_family = AF_INET;
    ((struct sockaddr_in*)remaddr)->sin_port = htons(port);
    _trudpUdpHostToIp((struct sockaddr_in*)remaddr, addr);

    return 0;
}

/**
 * Get address and port from address structure
 *
 * @param remaddr
 * @param port Pointer to port to get port integer
 * @return Pointer to address string
 */
char* trudpUdpGetAddr(__CONST_SOCKADDR_ARG remaddr, int* port) {
    char* addr = inet_ntoa(((struct sockaddr_in*)remaddr)->sin_addr); // IP to string
    if (port)
        *port = ntohs(((struct sockaddr_in*)remaddr)->sin_port); // Port to integer

    return addr;
}

/**
 * Create and bind UDP socket for client/server
 *
 * @param[in,out] port Pointer to Port number
 * @param[in] allow_port_increment_f Allow port increment flag
 * @return File descriptor or error if return value < 0:
 *         -1 - cannot create socket; -2 - can't bind on port
 */
int trudpUdpBindRaw(int* port, int allow_port_increment_f) {
    struct sockaddr_in addr; // Our address

    // Create an UDP socket
    int fd = _trudpUdpSocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) {
        perror("cannot create socket\n");
        return -1;
    }

    memset((char*)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the socket to any valid IP address and a specific port, increment
    // port if busy
    for (int i = 0;;) {
        addr.sin_port = htons(*port);

        // Try to bind
        if (_trudpUdpBind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            fprintf(stderr, "can't bind on port %d, try next port number ...\n", *port);
            (*port)++;
            if (allow_port_increment_f && i++ < NUMBER_OF_TRY_PORTS)
                continue;
            else
                return -2;
        }

        // Bind successfully
        else {
            if (!*port)
                trudpUdpGetAddr((__CONST_SOCKADDR_ARG)&addr, port);
            _trudpUdpSetNonblock(fd);
            break;
        }
    }

    return fd;
}

/**
 * Simple UDP recvfrom wrapper
 *
 * @param fd
 * @param buffer
 * @param buffer_size
 * @param remaddr
 * @param addr_length
 * @return
 */
ssize_t trudpUdpRecvfrom(
    int fd, uint8_t* buffer, size_t buffer_size, __SOCKADDR_ARG remaddr, socklen_t* addr_length) {
    int flags = 0;

    // Read UDP data
    ssize_t recvlen = recvfrom(fd, reinterpret_cast<char*>(buffer), buffer_size, flags,
        (__SOCKADDR_ARG)remaddr, addr_length);

    return recvlen;
}

/**
 * Simple UDP sendto wrapper
 *
 * @param fd File descriptor
 * @param buffer
 * @param buffer_size
 * @param remaddr
 * @param addrlen
 * @return
 */
ssize_t trudpUdpSendto(
    int fd, const uint8_t* buffer, size_t buffer_size, __CONST_SOCKADDR_ARG remaddr, socklen_t addrlen) {
    CLTRACK(
        trudpOpt_DBG_sendto, "TrUdp", "Sending %u bytes using sendto().", (uint32_t)buffer_size);

    ssize_t sendlen = 0;

    // if(waitSocketWriteAvailable(fd, 1000000) > 0) {

    // Write UDP data
    int flags = 0;
    sendlen = sendto(fd, reinterpret_cast<const char*>(buffer), buffer_size, flags, remaddr, addrlen);
    //}

    if (sendlen == -1) {
        int err = errno;
        LTRACK_E("TrUdp", "Sending %u bytes using sendto() failed with error %d.",
            (uint32_t)buffer_size, err);
    } else if ((size_t)sendlen != buffer_size) {
        LTRACK_E("TrUdp", "Sending using sendto() sent only %u bytes of %u.", (uint32_t)sendlen,
            (uint32_t)buffer_size);
    }

    return sendlen;
}
