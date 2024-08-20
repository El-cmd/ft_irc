#include "../include/client.hpp"

client::client(int fd): _fd(fd)
{
    _auth = false;
}

client::~client()
{
}

client::client()
{}




