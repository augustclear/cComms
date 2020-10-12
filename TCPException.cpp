#include "TCPException.hpp"

const char *TCPException::what() const throw()
{
    return "TCP Exception";
}

const char *TCPException::cause()
{
    return "Unknown";
}

const char *socket_failed::cause()
{
    return "Socket Failed to Initialize";
}

const char *setsockopt_failed::cause()
{
    return "Could Not Set Socket Options";
}

const char *bind_failed::cause()
{
    return "Could Not Bind";
}

const char *listen_failed::cause()
{
    return "Could Not Listen";
}

const char *accept_failed::cause()
{
    return "Could Not Accept New Connection";
}

const char *read_failed::cause()
{
    return "Could Not Read (No Data/Bad Read)";
}

const char *write_failed::cause()
{
    return "Could Not Write";
}

const char *connect_failed::cause()
{
    return "Could Not Connect to Server";
}

const char *invalid_address::cause()
{
    return "Address is Not Valid";
}