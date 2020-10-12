#include "Server.hpp"

Server::Server(int port) : _port(port)
{
    int opt = 1;
    try
    {
        if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            throw socket_failed();
        }
        if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                       &opt, sizeof(opt)) < 0)
        {
            throw setsockopt_failed();
        }
        _serveraddr.sin_family = AF_INET;
        _serveraddr.sin_addr.s_addr = INADDR_ANY;
        _serveraddr.sin_port = htons(_port);
    }
    catch (TCPException &e)
    {
        cout << "[SERVER] EXCEPTION: " << e.cause() << endl;
        shutdown();
    }
}

Server::~Server()
{
    cout << "[SERVER] Destroying server..." << endl;
    close(_sockfd);
    close(_clientfd);
}

int Server::get_port() const
{
    return _port;
}

void Server::init()
{
    cout << "[SERVER] Initializing server..." << endl;

    try
    {
        if (bind(_sockfd, (struct sockaddr *)&_serveraddr,
                 sizeof(_serveraddr)) < 0)
        {
            throw bind_failed();
        }
        if (listen(_sockfd, 3) < 0)
        {
            throw listen_failed();
        }
    }
    catch (TCPException &e)
    {
        cout << "[SERVER] EXCEPTION: " << e.cause() << endl;
        shutdown();
    }
    cout << "[SERVER] Initialized." << endl;
}

void Server::shutdown()
{
    cout << "[SERVER] Shutting down..." << endl;
    close(_sockfd);
    close(_clientfd);
}

void Server::handleNewConnection()
{
    int addrlen = sizeof(_clientaddr);
    if ((_clientfd = accept(_sockfd, (struct sockaddr *)&_clientaddr,
                            (socklen_t *)&addrlen)) < 0)
    {
        throw accept_failed();
    }
    cout << "[SERVER] New Connection (" << _clientfd << ")" << endl;
    cout << "IP:" << inet_ntoa(_clientaddr.sin_addr) << " PORT:" << _clientaddr.sin_port << endl;
}

Message Server::recieve(unsigned int wait_s)
{
    //Used in select, the set of file descriptors that can be read
    fd_set readfds;
    FD_SET(_clientfd, &readfds);
    char lengthbytes[2];
    short msglength;

    //Sets timeout for select, 0 for polling
    timeval timeout;
    timeout.tv_sec = wait_s;
    timeout.tv_usec = 0;

    //Check if there is a message from client, if not throw exception
    if (select(_clientfd + 1, &readfds, NULL, NULL, &timeout) > 0)
    {
        //Read first two bytes to get length
        read(_clientfd, &lengthbytes, 2);

        //Convert from bytes to short
        msglength = (lengthbytes[1] << 8) | lengthbytes[0];
        
        //set a buffer large enough to read in message
        char buffer[msglength];

        //Read message
        read(_clientfd, &buffer, msglength);

        //Returns unflattened message
        return Message(buffer);
    }
    throw read_failed();
}

void Server::deliver(Message msg)
{
    //The string form of the message to send
    string flatmsg = msg.flatten();

    //The length of the message's string form
    short datalength = flatmsg.length();

    //Buffer should be 2 bytes longer than message length
    short buffsize = datalength + 2;

    //Declare buffer big enough for message and length bytes
    char buffer[buffsize];

    //Convert length to bytes
    char lengthbytes[2];
    lengthbytes[0] = datalength;
    datalength = datalength >> 8;
    lengthbytes[1] = datalength;

    //Append length bytes
    buffer[0] = lengthbytes[0];
    buffer[1] = lengthbytes[1];

    //Append the rest of the message
    for (int i = 0; i <= buffsize-2; i++)
    {
        buffer[i+2] = flatmsg[i];
    }
    //Send message, if write is incomplete, throw exception
    if (send(_clientfd, &buffer, buffsize, 0) < buffsize)
    {
        throw write_failed();
    }
}