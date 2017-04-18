#include <string>
#include <cstring>
#include <map>

#include "server/Response.hpp"
#include "server/TcpConnection.hpp"
#include "http/HttpStatus.hpp"
#include "error/ResponseError.hpp"
#include "error/TodoError.hpp"
#include "Config.hpp"

Response::Response(Config const& config, TcpConnection& conn) :
    m_config(config),
    m_conn(conn),
    m_headers_sent(false)
{
    // We want every response to have this header
    // It tells browsers that we want separate connections per request
    m_headers["Connection"] = "close";
}

void Response::send(void const* buf, size_t bufsize, bool raw)
{
    throw TodoError("2", "You need to implement sending responses");
}

void Response::send_headers()
{
    throw TodoError("2", "You need to implement sending headers");
}

void Response::set_header(std::string const& key, std::string const& value)
{
    throw TodoError("2", "You need to implement controllers setting headers");
}

void Response::set_status(HttpStatus const& status)
{
    m_status_text = status.to_string();
}
