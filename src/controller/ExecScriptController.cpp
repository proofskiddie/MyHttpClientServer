#include <string>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <climits>
#include <cstdlib>
#include <vector>

#include "Config.hpp"
#include "Utils.hpp"
#include "controller/Controller.hpp"
#include "controller/ExecScriptController.hpp"
#include "server/Request.hpp"
#include "server/Response.hpp"
#include "http/HttpStatus.hpp"
#include "error/ControllerError.hpp"
#include "error/TodoError.hpp"

ExecScriptController::ExecScriptController(Config const& config, std::string const& ignore) :
    Controller(config),
    m_ignore(ignore)
{

}

void ExecScriptController::run(Request const& req, Response& res) const
{

}

bool ExecScriptController::set_environment(Request const& req) const noexcept
{
    setenv("HTTP:METHOD", "POST");
    setenv("HTTP:PATH", req.get_path().c_str());
    for (auto const& element : req.get_query()) 
    	setenv(("HTTP:QUERY:" + element.first).c_str(), element.second.c_str()); 
    for (auto const& element : req.get_headers()) 
    	setenv(("HTTP:HEADER:" + element.first).c_str(), element.second.c_str()); 
    for (auto const& element : req.get_body()) 
    	setenv(("HTTP:BODY:" + element.first).c_str(), element.second.c_str()); 
}
