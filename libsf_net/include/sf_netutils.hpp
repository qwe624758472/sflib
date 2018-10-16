#pragma once

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
#endif

#include "sf_netutils.h"

namespace skyfire
{


    inline bool get_peer_addr(SOCKET sock, addr_info_t &addr) {
#ifdef _WIN32
        SOCKADDR_IN sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        int len = sizeof(sock_addr);
        if(getpeername(sock, reinterpret_cast<SOCKADDR*>(&sock_addr), &len) != 0){
            return false;
        }
        addr.ip = inet_ntoa(sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#else
        sockaddr_in sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        socklen_t len = sizeof(sock_addr);
        if(getpeername(sock, reinterpret_cast<sockaddr*>(&sock_addr), &len) != 0){
            return false;
        }
        addr.ip = inet_ntoa(sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#endif
    }

    inline bool get_local_addr(SOCKET sock, addr_info_t &addr) {
#ifdef _WIN32
        SOCKADDR_IN sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        int len = sizeof(sock_addr);
        if(getsockname(sock, reinterpret_cast<SOCKADDR*>(&sock_addr), &len) != 0){
            return false;
        }
        addr.ip = inet_ntoa(sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#else
        sockaddr_in sock_addr;
        memset(&sock_addr,0, sizeof(sock_addr));
        socklen_t len = sizeof(sock_addr);
        if(getsockname(sock, reinterpret_cast<sockaddr*>(&sock_addr), &len) != 0){
            return false;
        }
        addr.ip = inet_ntoa(sock_addr.sin_addr);
        addr.port = ntohs(sock_addr.sin_port);
        return true;
#endif
    }

}