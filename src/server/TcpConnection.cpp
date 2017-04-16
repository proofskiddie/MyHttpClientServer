#include <string>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>

#include "Utils.hpp"
#include "Config.hpp"
#include "server/TcpConnection.hpp"
#include "error/ConnectionError.hpp"
#include "error/SocketError.hpp"
#include "error/TodoError.hpp"

TcpConnection::TcpConnection(Config const& config, int master_fd) :
    m_config(config),
    m_master(master_fd),
    m_shutdown(false)
{
    accept(m_master, NULL, 0);
}

TcpConnection::~TcpConnection() noexcept
{
    d_printf("Closing connection on %d", m_conn);

    if (close(m_conn) == -1) d_errorf("Could not close connection %d", m_conn);
}

void TcpConnection::shutdown()
{
    d_printf("Shutting down connection on %d", m_conn);
    
    if (::shutdown(m_conn, SHUT_RDWR) == -1) d_errorf("Could not shut down connection %d", m_conn);

    m_shutdown = true;
}

bool TcpConnection::getc(unsigned char* c)
{
	if (*c = fgetc(m_master) == EOF)
		return false;
	return true;
}

void TcpConnection::putc(unsigned char c)
{
    throw TodoError("2", "You need to implement writing characters to connections");
}

void TcpConnection::puts(std::string const& str)
{
    throw TodoError("2", "You need to implement writing strings to connections");
}

void TcpConnection::putbuf(void const* buf, size_t bufsize)
{
    throw TodoError("2", "You need to implement writing buffers to connections");
}
