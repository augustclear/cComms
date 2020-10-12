#pragma once

#include <stdexcept>

using namespace std;

struct TCPException : public exception
{
    const char *what() const throw();
    virtual const char *cause();
};

struct socket_failed : public TCPException
{
    const char *cause() override;
};

struct setsockopt_failed : public TCPException
{
    const char *cause() override;
};

struct bind_failed : public TCPException
{
    const char *cause() override;
};

struct listen_failed : public TCPException
{
    const char *cause() override;
};

struct accept_failed : public TCPException
{
    const char *cause() override;
};

struct read_failed : public TCPException{
    const char *cause() override;
};

struct write_failed : public TCPException{
    const char *cause() override;
};

struct connect_failed : public TCPException{
    const char *cause() override;
};

struct invalid_address : public TCPException{
    const char *cause() override;
};