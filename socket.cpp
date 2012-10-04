//
//  socket.cpp
//
//  Created by Jeong YunWon on 12. 9. 7..
//
/*****************************************************************************
 Copyright (c) 2012 The Board of Trustees of the University of Illinois.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 
 * Redistributions of source code must retain the above
 copyright notice, this list of conditions and the
 following disclaimer.
 
 * Redistributions in binary form must reproduce the
 above copyright notice, this list of conditions
 and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of the University of Illinois
 nor the names of its contributors may be used to
 endorse or promote products derived from this
 software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <arpa/inet.h>
#include <string.h>

#include "socket.h"

#ifndef dlog
	#define dlog(...)
#endif

namespace UDT {
    struct sockaddr_in SockAddrMake(const char *ip, uint16_t port);
    struct sockaddr_in SockAddrMake(const unsigned char *binip, uint16_t port);
}

struct sockaddr_in UDT::SockAddrMake(const char *ip, uint16_t port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (ip != NULL) {
        inet_pton(AF_INET, ip, &addr.sin_addr);
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
    }

    memset(&(addr.sin_zero), '\0', 8);
    return addr;
}

struct sockaddr_in UDT::SockAddrMake(const unsigned char *binip, uint16_t port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (binip != NULL) {
        memcpy((void*)&addr.sin_addr, binip, 4);
    } else {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    
    memset(&(addr.sin_zero), '\0', 8);
    return addr;
}

UDT::Socket::Socket() {
    this->udtsocket = UDT::socket(AF_INET, SOCK_STREAM, 0);
    close_on_delete = true;
    
    int buf_size = UDT_DEFAULT_BUFFER;
    UDT::setsockopt(this->udtsocket, 0, UDP_SNDBUF, &buf_size, sizeof(int));
    UDT::setsockopt(this->udtsocket, 0, UDP_RCVBUF, &buf_size, sizeof(int));
}

UDT::Socket::Socket(int af, int type, int protocol) {
    this->udtsocket = UDT::socket(af, type, protocol);
    close_on_delete = true;
    
    int buf_size = UDT_DEFAULT_BUFFER;
    UDT::setsockopt(this->udtsocket, 0, UDP_SNDBUF, &buf_size, sizeof(int));
    UDT::setsockopt(this->udtsocket, 0, UDP_RCVBUF, &buf_size, sizeof(int));
}

UDT::Socket::Socket(UDTSOCKET udtsocket, bool close_on_delete) {
    this->udtsocket = udtsocket;
    this->close_on_delete = close_on_delete;
}

UDT::Socket::~Socket() {
    if (close_on_delete) {
        UDT::close(this->udtsocket);
    }
}

bool UDT::Socket::_update_error(int res) {
    if (res == UDT::ERROR) {
        // NOTE: trace stake here!
        static int counter = 0;
        counter++;
        this->error = &UDT::getlasterror();
        dlog(this->error->getErrorCode() != 6002 && counter % 20 == 0, "Socket Error! %d %s", this->error->getErrorCode(), this->error->getErrorMessage());
        return true;
    }
    this->error = NULL;
    return false;
}

int UDT::Socket::error_code() {
    if (this->error == NULL) {
        return 0;
    }
    if (this->error != &UDT::getlasterror()) {
        return -1;
    }
    return this->error->getErrorCode();
}

const char *UDTSocketErrorExpired = "Error was captured but expired";
const char *UDT::Socket::error_message() {
    if (this->error == NULL) {
        return NULL;
    }
    if (this->error != &UDT::getlasterror()) {
        return UDTSocketErrorExpired;
    }
    return this->error->getErrorMessage();
}

void UDT::Socket::set_option(SOCKOPT option, int value) {
    UDT::setsockopt(this->udtsocket, 0, option, &value, sizeof(value));
}

void UDT::Socket::set_option(SOCKOPT option, bool value) {
    UDT::setsockopt(this->udtsocket, 0, option, &value, sizeof(value));    
}

bool UDT::Socket::bind(UDT::Socket *socket) {
    sockaddr addr;
    int addr_len;
    UDT::getsockname(socket->udtsocket, &addr, &addr_len);
    int res = UDT::bind(this->udtsocket, (sockaddr *)&addr, addr_len);
    return !this->_update_error(res);
}

bool UDT::Socket::bind(const char *ip, uint16_t port) {
    sockaddr_in addr = UDT::SockAddrMake(ip, port);
    
    int res = UDT::bind(this->udtsocket, (sockaddr *)&addr, sizeof(addr));
    return !this->_update_error(res);
}

bool UDT::Socket::bind(const unsigned char *ip, uint16_t port) {
    sockaddr_in addr = UDT::SockAddrMake(ip, port);
    
    int res = UDT::bind(this->udtsocket, (sockaddr *)&addr, sizeof(addr));
    return !this->_update_error(res);
}

bool UDT::Socket::listen(int backlog) {
    int res = UDT::listen(this->udtsocket, backlog);
    return !this->_update_error(res);
}

UDT::Socket *UDT::Socket::accept() {
    struct sockaddr addr;
    int addrlen;
    UDTSOCKET socket = UDT::accept(this->udtsocket, &addr, &addrlen);
    if (this->_update_error(socket)) {
        return NULL;
    }
    return new UDT::Socket(socket);
}

bool UDT::Socket::connect(const char *ip, uint16_t port) {
    sockaddr_in addr = UDT::SockAddrMake(ip, port);
    
    int res = UDT::connect(this->udtsocket, (sockaddr *)&addr, sizeof(addr));
    return !this->_update_error(res);
}

void UDT::Socket::close() {
    UDT::close(this->udtsocket);
    close_on_delete = false;
}

size_t UDT::Socket::send(const char *data, size_t len) {
    size_t size = UDT::send(this->udtsocket, data, len, 0);
//    if (this->_update_error(size)) {
//        return 0;
//    }
    return size;
}

size_t UDT::Socket::send(void *data, size_t len) {
    return this->send((const char *)data, len);
}

size_t UDT::Socket::recv(char *buffer, size_t buffer_len) {
    size_t size = UDT::recv(this->udtsocket, buffer, buffer_len, 0);
    if (this->_update_error(size)) {
        return 0;
    }
    return size;
}

size_t UDT::Socket::recv(void *buffer, size_t buffer_len) {
    return this->recv((char *)buffer, buffer_len);
}


