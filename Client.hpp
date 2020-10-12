#pragma once

#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h>
#include <string>
#include <cstring>

#include "Message.hpp"
#include "TCPException.hpp"

using namespace std;

class Client{
public:
    Client(string address, int port);
    ~Client();

    int get_port() const;

    void start();
    void stop();
    Message recieve(unsigned int wait_s = 0);
    void deliver(Message msg);

private:
    int _port, _sockfd;
    struct sockaddr_in _serveraddr;

    void handleNewConnection();
};