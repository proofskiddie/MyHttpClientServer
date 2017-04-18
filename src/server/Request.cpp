#include <unordered_map>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <stdexcept>

#include "server/Request.hpp"
#include "http/HttpStatus.hpp"
#include "server/TcpConnection.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "error/RequestError.hpp"
#include "error/ConnectionError.hpp"
#include "error/TodoError.hpp"
#include "../parse/lex.yy.c"
#include "../parse/y.tab.c"


void Request::parse_method(std::string& raw_line)
{
    throw TodoError("2", "You have to implement parsing methods");
}
Request(Config const& config, const TcpConnection *conn)
{
    m_config = config;
    m_conn = conn;
    std::string request_line = parse_raw_line();
    /*
    yy_scan_string(request_line.c_str());
    yylex();
    yyparse();
    yy_delete_buffer(YY_CURRENT_BUFFER);
    */
    //parse_method(request_line);
    //parse_route(request_line);
    //parse_version(request_line);

    // the previous three parse_* calls should consume the entire line
    if (!request_line.empty())
    {
        throw RequestError(HttpStatus::BadRequest, "Malformed request-line\n");
    }

    //parse_headers();
    //parse_body();
}

void Request::parse_route(std::string& raw_line)
{
    throw TodoError("2", "You have to implement parsing routes");
}

void Request::parse_querystring(std::string query, std::unordered_map<std::string, std::string>& parsed)
{
    throw TodoError("6", "You have to implement parsing querystrings");
}

void Request::parse_version(std::string& raw_line)
{
    throw TodoError("2", "You have to implement parsing HTTP version");
}

void Request::parse_headers()
{
    throw TodoError("2", "You have to implement parsing headers");
}

void Request::parse_body()
{
    if (m_method == "GET") return;

    throw TodoError("6", "You have to implement parsing request bodies");
}

std::string Request::parse_raw_line()
{
	unsigned char c;
	std::string s;
	while (m_conn->getc(&c))
		s += c;
	return s;
}

void Request::print() const noexcept
{
    std::cout << m_method << ' ' << m_path << ' ' << m_version << std::endl;
#ifdef DEBUG    
    for (auto const& el : m_headers)
    {
        std::cout << el.first << ": " << el.second << std::endl;
    }

    for (auto const& el : m_query)
    {
        std::cerr << el.first << ": " << el.second << std::endl;
    }

    for (auto const& el : m_body_data)
    {
        std::cerr << el.first << ": " << el.second << std::endl;
    }
#endif	
}

bool Request::try_header(std::string const& key, std::string& value) const noexcept
{
    if (m_headers.find(key) == m_headers.end())
    {
        return false;
    }
    else
    {
        value = m_headers.at(key);
        return true;
    }
}

std::string const& Request::get_path() const noexcept
{
    return m_path;
}

std::string const& Request::get_method() const noexcept
{
    return m_method;
}

std::string const& Request::get_version() const noexcept
{
    return m_version;
}

std::unordered_map<std::string, std::string> const& Request::get_headers() const noexcept
{
    return m_headers;
}

std::unordered_map<std::string, std::string> const& Request::get_query() const noexcept
{
    return m_query;
}

std::unordered_map<std::string, std::string> const& Request::get_body() const noexcept
{
    return m_body_data;
}
