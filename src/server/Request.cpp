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

Request::Request(Config *config, TcpConnection *conn)
{ 
    m_config = config;
    m_conn = conn;
    std::string request_line = parse_raw_line();
    if (request_line.size() != 0) {
	    parse_method(request_line);
	    parse_route(request_line);
	    parse_version(request_line);
	    parse_headers();
	    parse_body();
    }
    /*
    if (!request_line.empty())
    {
        throw RequestError(HttpStatus::BadRequest, "Malformed request-line\n");
    }
    */
}

void Request::parse_method(std::string& raw_line)
{
	if (!raw_line.substr(0,3).compare("GET")) {
		m_method = "GET";
		raw_line = raw_line.substr(3);
	} else if (!raw_line.substr(0,4).compare("POST")) {
		m_method = "POST";
		raw_line = raw_line.substr(4);
	} else
		throw RequestError(HttpStatus::MethodNotAllowed, "405 Method Not Allowed\n");
}

void Request::parse_route(std::string& raw_line)
{
	int i = 0;
	while (raw_line[i] == ' ' || raw_line[i] == '\t') ++i;
	raw_line = raw_line.substr(i);
	bool err = false;
	if (raw_line[0] != '/') err = true;
	else 
		for (i = 0; raw_line[i] != ' ' && raw_line[i] != '\t'; ++i)
			if (raw_line[i] == '\n' || raw_line[i] == '\r')
				err = true;
	if (err)
		throw RequestError(HttpStatus::BadRequest, "400 Bad Request\n");
	else {
		m_path = raw_line.substr(0,i);
		raw_line = raw_line.substr(i);
	}	
}

void Request::parse_querystring(std::string query, std::unordered_map<std::string, std::string>& parsed)
{
}

void Request::parse_version(std::string& raw_line)
{
	int i = 0;
	while (raw_line[i] == ' ' || raw_line[i] == '\t') ++i;
	raw_line = raw_line.substr(i);
	
	if (!raw_line.substr(0,8).compare("HTTP/1.0")) {
		m_version = "HTTP/1.0";
		raw_line = raw_line.substr(8);
	} else if (!raw_line.substr(0,8).compare("HTTP/1.1")) {
		m_version = "HTTP/1.1";
		raw_line = raw_line.substr(8);
	} else
		throw RequestError(HttpStatus::HttpVersionNotSupported, "505 HTTP Version Not Supported\n");
}

void Request::parse_headers()
{
	
}

void Request::parse_body()
{
    if (m_method == "GET") return;

}

std::string Request::parse_req_line()
{
	unsigned char c;
	int bsen = 0;
	std::string s;
	while (m_conn->getc(&c)) {
		s += c;
		if ((bsen == 0 || bsen == 2) && c == '\r') ++bsen;
		else if ((bsen == 1 || bsen == 3) && c == '\n') {
			++bsen;
			if (bsen == 4) return s;
		} else bsen = 0;
	}
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
