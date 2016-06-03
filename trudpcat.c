/*
 * The MIT License
 *
 * Copyright 2016 Kirill Scherba <kirill@scherba.ru>.
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

/* 
 * File:   trudpcat.c
 * Author: Kirill Scherba <kirill@scherba.ru>
 *
 * Created on June 2, 2016, 1:11 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>

// C11 present
#if __STDC_VERSION__ >= 201112L
extern int usleep (__useconds_t __useconds);
#endif

#include "libtrudp/tr-udp.h"

// Integer options
int o_debug = 0,
    o_listen = 0,
    o_numeric = 0,
    o_buf_size = 4096;

// String options
char *o_local_address = NULL,  
     *o_local_port = NULL,
     *o_remote_address = NULL, 
     *o_remote_port = NULL;

int o_remote_port_i;

// Application exit code and flags
int exit_code = EXIT_SUCCESS,
    quit_flag = 0;

// Read buffer
char *buffer;

/**
 * Show error and exit
 * 
 * @param fmt
 * @param ...
 */
static void die(char *fmt, ...)
{
	va_list ap;
	fflush(stdout);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

/**
 * Show debug message
 * 
 * @param fmt
 * @param ...
 */
void debug(char *fmt, ...)
{
    static unsigned long idx = 0;
    va_list ap;
    if (o_debug) {
            fflush(stdout);
            fprintf(stderr, "%lu debug: ", ++idx);
            va_start(ap, fmt);
            vfprintf(stderr, fmt, ap);
            va_end(ap);
            fflush(stderr);
    }
}

/**
 * TR-UDP process data callback
 * 
 * @param data
 * @param data_length
 * @param user_data
 */
static void processDataCb(void *td_ptr, void *data, size_t data_length, 
        void *user_data) {
    
    debug("got %d byte data, id=%u: ", (int)data_length, 
                trudpPacketGetId(trudpPacketGetPacket(data)));
    
    printf("%s\n",(char*)data);    
    debug("\n");
}

/**
 * TR-UDP ACK processed callback
 * 
 * @param td
 * @param data
 * @param data_length
 * @param user_data
 */
static void processAckCb(void *td_ptr, void *data, size_t data_length, void *user_data) {
    
    trudpData *td = (trudpData *)td_ptr;
    
    debug("got ACK id=%u processed %.3f(%.3f) ms\n", 
           trudpPacketGetId(trudpPacketGetPacket(data)), 
           (td->triptime)/1000.0, (td->triptimeMidle)/1000.0  );
}

/**
 * TR-UDP write callback
 * 
 * @param packet
 * @param packet_length
 * @param user_data
 */
static void writeCb(void *td_ptr, void *packet, size_t packet_length, void *user_data) {
    
    trudpData *td = (trudpData *)td_ptr;
    int type = trudpPacketGetDataType(packet);
    
    // Write to UDP
    if(td->connected_f) {
        trudpUdpSendto(td->fd, packet, packet_length, (__CONST_SOCKADDR_ARG) &td->remaddr, sizeof(td->remaddr));
        uint32_t id = trudpPacketGetId(packet);
        if(!type)
            debug("send %d bytes, id=%u\n", (int)packet_length, id);
        else 
            debug("send %d bytes %s id=%u\n", (int)packet_length, type == 1 ? "ACK":"RESET",  id);
    }
}

/**
 * The TR-UDP cat network loop
 */
void network_loop(trudpData *td) {
    
    // Read from UDP
    struct sockaddr_in remaddr; // remote address
    socklen_t addr_len = sizeof(remaddr);
    ssize_t recvlen = trudpUdpRecvfrom(td->fd, buffer, o_buf_size, (__SOCKADDR_ARG)&remaddr, &addr_len);    
    
    // Process received packet
    if(recvlen > 0) {
        
        if(!td->connected_f) {
            memcpy(&td->remaddr, &remaddr, addr_len);
            td->addrlen = addr_len;
            td->connected_f = 1;
        }
        
        size_t data_length;
        void *rv = trudpProcessReceivedPacket(td, buffer, recvlen, &data_length);
    }
    
    // Process send queue
    trudpProcessSendQueue(td);
}

/**
 * Show usage screen
 * 
 * @param name Start command to show in usage screen
 */
static void usage(char *name) {

	fprintf(stderr, "\nUsage:\n");
	fprintf(stderr, "    %s [options] <destination-IP> <destination-port>\n", name);
	fprintf(stderr, "    %s [options] -l -p <listening-port>\n", name);
	fprintf(stderr, "\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "    -h          Help\n");
	fprintf(stderr, "    -d          Debug mode; use multiple times to increase verbosity.\n");
	fprintf(stderr, "    -l          Listen mode\n");
	fprintf(stderr, "    -p <port>   Local port\n");
	fprintf(stderr, "    -s <IP>     Source IP\n");
	fprintf(stderr, "    -B <size>   Buffer size\n");
//	fprintf(stderr, "    -n          Don't resolve hostnames\n");
	fprintf(stderr, "\n");
	exit(1);
}

/**
 * Main application function
 * 
 * @param argc
 * @param argv
 * @return 
 */
int main(int argc, char** argv) {
    
    int i;
    o_local_port = "8000"; // Default local port
    o_local_address = "0.0.0.0"; // Default local address

    // Read parameters
    while(1) {
            int c = getopt (argc, argv, "hdlp:B:s:n");
            if (c == -1) break;
            switch(c) {
                case 'h': usage(argv[0]);               break;
                case 'd': o_debug++;			break;
                case 'l': o_listen++;			break;
                case 'p': o_local_port = optarg;	break;
                case 'B': o_buf_size = atoi(optarg);	break;
                case 's': o_local_address = optarg;	break;
//                case 'n': o_numeric++;		  break;
                //case 'w': break;	// timeout for connects and final net reads
                default:  die("Unhandled argument: %c\n", c); break;
            }
    }

    // Read arguments
    for(i = optind; i < argc; i++) {
        switch(i - optind) {
            case 0:	o_remote_address = argv[i]; 	break;
            case 1:	o_remote_port = argv[i];	break;
        }
    }
    
    // Check necessary arguments
    if(o_listen && (o_remote_port || o_remote_address)) usage(argv[0]);
    if(!o_listen && (!o_remote_port || !o_remote_address)) usage(argv[0]);

    // Show logo
    fprintf(stderr, "TR-UDP two node connect sample application ver 0.0.1\n");
    
    // Show execution mode
    if(o_listen) 
        fprintf(stderr, "Server started at %s:%s\n", o_local_address, o_local_port);    
    else {
        o_remote_port_i = atoi(o_remote_port);
        fprintf(stderr, "Client start connection to %s:%d\n", o_remote_address, o_remote_port_i);
    }
    
    // Create read buffer
    buffer = malloc(o_buf_size);
    
    // Startup windows socket library
    #if defined(HAVE_MINGW) || defined(_WIN32) || defined(_WIN64)
    WSADATA wsaData;
    WSAStartup(0x0202, &wsaData);
    #endif     
    
    // Initialize TR-UDP
    trudpData *td = trudpNew(NULL, processDataCb, writeCb);
    trudpSetProcessAckCb(td, processAckCb);
    
    // Bind UDP port and get FD
    int port = atoi(o_local_port);
    td->fd = trudpUdpBindRaw(&port, 1);
    if(td->fd <= 0) die("Can't bind UDP port ...\n");
    else fprintf(stderr, "Start listening at port %d\n", port);
    
    // Create remote address and Send "connect" packet
    if(!o_listen) {
        char *connect = "Connect with TR-UDP!";
        size_t connect_length = strlen(connect) + 1;
        trudpUdpMakeAddr(o_remote_address, o_remote_port_i, (__SOCKADDR_ARG) &td->remaddr, &td->addrlen);
        td->connected_f = 1;
        trudpSendData(td, connect, connect_length);
    }

    char *hello_c = "Hello TR-UDP from client!";
    size_t hello_c_length = strlen(hello_c) + 1;
    
    char *hello_s = "Hello TR-UDP from server!";
    size_t hello_s_length = strlen(hello_s) + 1;
    
    i = 0; 
    int delay = 50;
    char *message;
    size_t message_length;
    if(!o_listen) { message = hello_c; message_length = hello_c_length; }    
    else { message = hello_s; message_length = hello_s_length; }
    uint32_t tt, tt_s = 0;
    while (!quit_flag) {
        
        network_loop(td);
        tt = trudpHeaderTimestamp();
        // Send message
        if((!o_listen || o_listen && td->connected_f) && (tt - tt_s)>1000000) {                    
          trudpSendData(td, message, message_length);
          tt_s = tt; 
        }
        
        usleep(delay);
        i++;
    }    
    
    // Destroy TR-UDP
    trudpDestroy(td);
    free(buffer);
    
    printf("Executed!\n");
    
    // Cleanup socket library
    #if defined(HAVE_MINGW) || defined(_WIN32) || defined(_WIN64)
    WSACleanup();
    #endif        
    
    return (EXIT_SUCCESS);
}