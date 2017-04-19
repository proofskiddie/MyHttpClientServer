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
    if (m_conn = accept(m_master, 0, 0), m_conn == -1)
	throw SocketError("accept");
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

bool TcpConnection::getc(unsigned char* c) const
{	
	int ret;
	if (ret = recv(m_master, c, 1, MSG_WAITALL), ret < 0)
		throw SocketError("getc");
	return (ret == 0)? false : true;
}

void TcpConnection::putc(unsigned char c)
{
	if (write(m_master, &c, 1) == -1)
		throw SocketError("putc");
}

void TcpConnection::puts(std::string const& str)
{
	if (write(m_master, str.c_str(), str.size()) == -1)
		throw SocketError("puts");		
}

void TcpConnection::putbuf(void const* buf, size_t bufsize)
{
	if (write(m_master, buf, bufsize) == -1)
		throw SocketError("putbuf");
}
