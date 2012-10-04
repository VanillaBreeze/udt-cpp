//
//  socket.h
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

#include <udt/udt.h>

// shallow cpp wrapper of UDT4

#define UDT_DEFAULT_BUFFER 4096

namespace UDT {
    /** @class
     *  @abstract UDT::Socket is shallow object-oriented model for UDTSOCKET
     *
     *  This is designed to provide pretty C++ interface for UDTSOCKET.
     *  This is easily adapted for raw polling, but not a good choice for
     *  select or epoll.
     */
    class Socket {
    public:
        UDTSOCKET udtsocket; // publicise raw socket before being mature
    private:
        bool close_on_delete;
        UDT::ERRORINFO *error;
        
        bool _update_error(int res);
    public:
        Socket();
        Socket(int af, int type, int protocol);
        Socket(UDTSOCKET udtsocket, bool close_on_delete=true);
        virtual ~Socket();
        
        void set_option(SOCKOPT option, int value);
        void set_option(SOCKOPT option, bool value);
        
        // NOTE: error handling of this wrapper is not concrete yet
        int error_code();
        const char *error_message();
        
        bool bind(Socket *socket);
        bool bind(const char *ip, uint16_t port);
        bool bind(const unsigned char *binip, uint16_t port);
        bool listen(int backlog=65535);
        bool connect(const char *ip, uint16_t port);
        void close();
        Socket *accept();
        
        size_t send(const char *data, size_t len);
        size_t send(void *data, size_t len);
        size_t recv(char *buffer, size_t buffer_len);
        size_t recv(void *buffer, size_t buffer_len);
    };
}
