#pragma once

#define MAX_CLIENTS 30

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <arpa/inet.h> 
#include <array>

#include "Message.hpp"
#include "TCPException.hpp"

using namespace std;

class Server
{

public:
    Server(int port);
    ~Server();

    int get_port() const;

    void init();
    void handleNewConnection();
    void shutdown();
    Message recieve(unsigned int wait_s = 0);
    void deliver(Message msg);

private:
    int _port, _sockfd, _clientfd;
    struct sockaddr_in _serveraddr, _clientaddr;
};