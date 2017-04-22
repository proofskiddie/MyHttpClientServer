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
    int child_stdout[2];
    if (pipe(child_stdout) == -1)
    {
    }
    pid_t child = fork();
    if (child == -1)
    {
    }
    if (child == 0)
    {
        if (dup2(child_stdout[1], STDOUT_FILENO) == -1)
        {
            exit(1);
        }
	set_environment(req);	
	const char *path = (m_config.exec_dir + req.get_path()).c_str();
        execlp(path, path, 0);
        exit(1);
    }

    if (close(child_stdout[1]) == -1)
    {
    }

    int status;
    if (waitpid(child, &status, 0) == -1)
    {
    }

    size_t const max_content_length = 500000;
    char buf[max_content_length];
    memset(buf, 0, max_content_length);

    if (read(child_stdout[0], buf, max_content_length - 1) == -1)
    {
    }

    if (close(child_stdout[0]) == -1)
    {
    }

    // remove the trailing newline before returning
    std::string content_type(buf);
    content_type = content_type.substr(0, content_type.size() - 1);
	const char *path = (m_config.exec_dir + req.get_path()).c_str();
	
		std::fstream fs(path);
		if (!fs)
			return;
		int length = get_content_length(fs);
		if (length <= 0) return;
    res.set_status(HttpStatus::Ok);
    res.set_header("Content-Length", std::to_string(length));
    res.set_header("Content-Type", get_content_type(path));
    res.send(content_type.c_str(), (size_t)content_type.length());
}
int ExecScriptController::get_content_length(std::fstream& fs) const
{
	fs.seekg(0, fs.end);
	int len = fs.tellg();
	fs.seekg(0, fs.beg);
	return len;
}

bool ExecScriptController::set_environment(Request const& req) const noexcept
{
    int err;
    err = setenv("HTTP:METHOD", "POST", 1);
    err = setenv("HTTP:PATH", req.get_path().c_str(), 1);
    for (auto const& element : req.get_query()) 
    	err = setenv(("HTTP:QUERY:" + element.first).c_str(), element.second.c_str(), 1); 
    for (auto const& element : req.get_headers()) 
    	err = setenv(("HTTP:HEADER:" + element.first).c_str(), element.second.c_str(), 1); 
    for (auto const& element : req.get_body()) 
    	err = setenv(("HTTP:BODY:" + element.first).c_str(), element.second.c_str(), 1); 
    return (err == -1)? false : true;
}
