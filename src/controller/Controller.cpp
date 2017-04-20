
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <climits>
#include <string>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <limits.h>

#include "controller/Controller.hpp"
#include "error/ControllerError.hpp"
#include "server/Request.hpp"
#include "server/Response.hpp"
#include "server/TcpConnection.hpp"
#include "http/HttpStatus.hpp"
#include "error/TodoError.hpp"
#include "Utils.hpp"

Controller::Controller(Config const& config) : m_config(config)
{
    
}

Controller::~Controller()
{
    
}

void Controller::send_error_response(Response& res, HttpStatus const& status, std::string response_text)
{
    try
    {
        res.set_status(status);
        res.set_header("Content-Type", "text/plain");
        res.set_header("Content-Length", std::to_string(response_text.size()));
        res.send(response_text.c_str(), response_text.size());
    }
    catch (std::runtime_error const& e)
    {
        d_warn("Could not write error response");
        d_warnf("Error: %s", e.what());
    }
}

void Controller::send_error_response(Config const& config, TcpConnection* conn, HttpStatus const& status, std::string response_text)
{
    try
    {
        Response res(config, *conn);
        res.set_status(status);
        res.set_header("Content-Type", "text/plain");
        res.set_header("Content-Length", std::to_string(response_text.size()));
        res.send(response_text.c_str(), response_text.size());
    }
    catch (std::runtime_error const& e)
    {
        d_warn("Could not write error response");
        d_warnf("Error: %s", e.what());
    }
}

bool Controller::resolve_requested_path(std::string const& requested, std::string const& basedir, std::string& resolved) const noexcept {}

std::string Controller::real_path (std::string const& basedir) const noexcept {
    int child_stdout[2];
    if (pipe(child_stdout) == -1)
    {
        throw ControllerError("Could not create pipe to communicate with `xdg-mime`");
    }

    d_printf("Getting content type. child_stdout = {%d, %d}", child_stdout[0], child_stdout[1]);

    pid_t child = fork();
    if (child == -1)
    {
        throw ControllerError("Could not create child process to find MIME type");
    }
    if (child == 0)
    {
        if (dup2(child_stdout[1], STDOUT_FILENO) == -1)
        {
            exit(1);
        }
	char *args[2];
	args[0] = strdup("realpath");
	args[1] = strdup(basedir.c_str());
        execlp("realpath", args, 0);
        exit(1);
    }

    if (close(child_stdout[1]) == -1)
    {
        throw ControllerError("Could not close pipe input");
    }

    int status;
    if (waitpid(child, &status, 0) == -1)
    {
        throw ControllerError("Error while waiting for finding MIME type to finish");
    }

    if ((WIFEXITED(status) && WEXITSTATUS(status) != 0) || WIFSIGNALED(status))
    {
        throw ControllerError("`xdg-mime` exited with errors");
    }

    size_t const max_content_length = 50;
    char buf[max_content_length];
    memset(buf, 0, max_content_length);

    if (read(child_stdout[0], buf, max_content_length - 1) == -1)
    {
        throw ControllerError("Could not read output of `xdg-mime`");
    }

    if (close(child_stdout[0]) == -1)
    {
        throw ControllerError("Could not close pipe output");
    }

    // remove the trailing newline before returning
    std::string content_type(buf);
    content_type = content_type.substr(0, content_type.size() - 1);

    return content_type;
}
