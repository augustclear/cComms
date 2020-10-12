#include "Client.hpp"

Client::Client(string address, int port) : _port(port)
{
    cout << "[CLIENT] Initializing client..." << endl;
    try
    {
        if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            throw socket_failed();
        }
        _serveraddr.sin_family = AF_INET;
        _serveraddr.sin_port = htons(_port);

        //Convert address to c string
        char c_address[address.length() + 1];
        strcpy(c_address, address.c_str());

        if (inet_pton(AF_INET, c_address, &_serveraddr.sin_addr) <= 0)
        {
            throw invalid_address();
        }
    }
    catch (TCPException &e)
    {
        cout << "[CLIENT] EXCEPTION: " << e.cause() << endl;
        stop();
    }
}
Client::~Client()
{
    cout << "[CLIENT] Destroying client..." << endl;
    close(_sockfd);
}

void Client::start()
{
    cout << "[CLIENT] Connecting..." << endl;
    try
    {
        if (connect(_sockfd, (struct sockaddr *)&_serveraddr, sizeof(_serveraddr)) < 0)
        {
            throw connect_failed();
        }
        _serveraddr.sin_family = AF_INET;
        _serveraddr.sin_port = htons(_port);
    }
    catch (TCPException &e)
    {
        cout << "[CLIENT] EXCEPTION: " << e.cause() << endl;
        stop();
    }
}

void Client::stop()
{
    cout << "[CLIENT] Disconnecting..." << endl;
    close(_sockfd);
}

int Client::get_port() const
{
    return _port;
}

Message Client::recieve(unsigned int wait_s)
{
    //Used in select, the set of file descriptors that can be read
    fd_set readfds;
    FD_SET(_sockfd, &readfds);
    
    char lengthbytes[2];
    short msglength;

    //Sets timeout for select, 0 for polling
    timeval timeout;
    timeout.tv_sec = wait_s;
    timeout.tv_usec = 0;
    //Check if there is a message from server, if not throw exception
    if (select(_sockfd + 1, &readfds, NULL, NULL, &timeout) > 0)
    {
        //Read first two bytes to get length
        read(_sockfd, &lengthbytes, 2);

        //Convert from bytes to short
        msglength = (lengthbytes[1] << 8) | lengthbytes[0];
        //set a buffer large enough to read in message
        char buffer[msglength];

        //Read message
        read(_sockfd, &buffer, msglength);
        //Returns unflattened message
        return Message(buffer);
    }
    throw read_failed();
}

void Client::deliver(Message msg)
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
    if (send(_sockfd, &buffer, buffsize, 0) < buffsize)
    {
        throw write_failed();
    }
}