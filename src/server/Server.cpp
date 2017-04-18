#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>
#include <stdexcept>
#include <signal.h>

#include "Utils.hpp"
#include "server/TcpConnection.hpp"
#include "server/Server.hpp"
#include "server/Request.hpp"
#include "server/Response.hpp"
#include "controller/Controller.hpp"
#include "controller/SendFileController.hpp"
#include "controller/TextController.hpp"
#include "controller/ExecScriptController.hpp"
#include "http/HttpStatus.hpp"
#include "error/RequestError.hpp"
#include "error/ResponseError.hpp"
#include "error/ControllerError.hpp"
#include "error/SocketError.hpp"
#include "error/ConnectionError.hpp"
#include "error/TodoError.hpp"

Server::Server(Config const& config) : m_config(config)
{
    throw TodoError("2", "Server constructor/connecting to a socket");
    
    m_master = socket(AF_INET, SOCK_STREAM, 0);
    if (m_master == -1) perror("error creating mastersocket");
    
    int optval = 1;
    setsockopt(m_master, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    
    int error = bind(m_master, (struct sockaddr *)&addr, sizeof(addr));
    if (error == -1) perror("error binding address to mastersocket");
    error = listen(m_master, config.queue_length);
    
}

void Server::run_linear() const
{
    while (true)
    {
        std::string response;
        TcpConnection* conn = new TcpConnection(m_config, m_master);

        handle(conn);

        delete conn;
    }
}

void Server::run_thread_request() const
{
    throw TodoError("3", "You need to implement thread-per-request mode");
}

void Server::run_fork() const
{
    throw TodoError("3", "You need to implement process-per-request mode");
}

void Server::run_thread_pool() const
{
    throw TodoError("3", "You need to implement thread-pool mode");
}

void Server::handle(TcpConnection* conn) const
{

    Controller const* controller = nullptr;

    try
    {
        // creating req will parse the incoming request
        Request req();
	reg.set_conn(*conn);
	reg.set_config(m_config);
        // creating res as an empty response
        Response res(m_config, *conn);

        // Printing the request will be helpful to tell what our server is seeing
        req.print();

        std::string path = req.get_path();

        // This will route a request to the right controller
        // You only need to change this if you rename your controllers or add more routes
        if (path == "/hello-world")
        {
            controller = new TextController(m_config, "Hello world!\n");
        }
        else if (path.find("/script") == 0)
        {
            controller = new ExecScriptController(m_config, "/script");
        }
        else
        {
            controller = new SendFileController(m_config);
        }

        // Whatever controller we picked needs to be run with the given request and response
        controller->run(req, res);
    }
    catch (RequestError const& e)
    {
        d_warnf("Error parsing request: %s", e.what());
        
        Controller::send_error_response(m_config, conn, e.status, "Error while parsing request\n");
    }
    catch (ControllerError const& e)
    {
        d_warnf("Error while handling request: %s", e.what());

        Controller::send_error_response(m_config, conn, HttpStatus::InternalServerError, "Error while handling request\n");
    }
    catch (ResponseError const& e)
    {
        d_warnf("Error while creating response: %s", e.what());

        Controller::send_error_response(m_config, conn, HttpStatus::InternalServerError, "Error while handling response\n");
    }
    catch (ConnectionError const& e)
    {
        // Do not try to write a response when we catch a ConnectionError, because that will likely just throw
        d_errorf("Connection error: %s", e.what());
    }
    catch (TodoError const& e)
    {
        d_errorf("You tried to use unimplemented functionality: %s", e.what());
    }

    // Dont forget about freeing memory!
    delete controller;
}

Server::~Server() noexcept
{
    if (close(m_master) == -1)
    {
        d_error("Could not close master socket");
    }
}

