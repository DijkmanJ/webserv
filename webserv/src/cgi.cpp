/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   cgi.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/10 12:29:42 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/19 12:26:30 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"


std::string server::getRequestBody(const std::string& req_str)
{
	try {
		size_t start_p = req_str.find("\r\n\r\n");
		std::string request_body = req_str.substr(start_p + 4);
		return (request_body);
	} catch (std::exception& e) { 
		logToFile("handleCGI - error occured in extracting request body, invalid request");
	}
	return ("");
}

void closeAllPipes(int pipe_in[2], int pipe_out[2])
{
	if (pipe_in[0] != -1)
		close(pipe_in[0]);
	if (pipe_in[1] != -1)
		close(pipe_in[1]);
	if (pipe_out[0] != -1)
		close(pipe_out[0]);
	if (pipe_out[1] != -1)
		close(pipe_out[1]);
}

void server::handleCGI(t_response *response, const std::string& req_str)
{
	std::string		path = response->request->uri.substr(1);
	pid_t			pid;
	int				pipe_in[2], pipe_out[2];
	auto			start_time = std::chrono::steady_clock::now();
	std::string		req_body = getRequestBody(req_str);


	if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
	{
		logToFile("handleCGI - failed to create pipe, sending ERROR 500");
		return (closeAllPipes(pipe_in, pipe_out), setErrorPageResponse(response, 500));
	}

	pid = fork();
	if (pid == -1)
	{
		logToFile("handleCGI - failed to fork process, sending ERROR 500");
		return (closeAllPipes(pipe_in, pipe_out), setErrorPageResponse(response, 500));
	}

	if (pid == 0)
	{
		if (dup2(pipe_in[0], STDIN_FILENO) == -1 || dup2(pipe_out[1], STDOUT_FILENO) == -1)
			exit(EXIT_FAILURE);
		char *const *envp = (char * const *)initEnv(response, req_body.length());
		if (!envp)
			exit(EXIT_FAILURE);
		char* const argv[] = {NULL, (char* const)path.c_str(), NULL};
		closeAllPipes(pipe_in, pipe_out);
		execve(path.c_str(), argv, envp);
		logToFile("handleCGI - execve failed in child process");
		exit(EXIT_FAILURE);
	}

	int child_status = 0;
	close(pipe_in[0]);
	close(pipe_out[1]);

	if (write(pipe_in[1], req_body.c_str(), req_body.length()) == -1)
		return (logToFile("handleCGI - write failed, sending 500"), setErrorPageResponse(response, 500));

	while (waitpid(pid, &child_status, WNOHANG) == 0)
	{
		auto current_time = std::chrono::steady_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
		if (elapsed_time.count() > TIME_OUT)
			return (logToFile("handleCGI - timeout occured, sending 500"), setErrorPageResponse(response, 500));
	}

	if (child_status == 256)
		return (logToFile("handleCGi - FAILED, child process EXIT_FAILURE, sending 500"), setErrorPageResponse(response, 500));
	
	response->content = readChildProcessOutPipe(pipe_out[0]);
	close(pipe_out[0]);
	close(pipe_in[1]);
	if (response->content.empty())
		return (setErrorPageResponse(response, 500));
	
	setContentType(response);
	setContentLength(response);
}

const char**	server::initEnv(t_response *response, int body_size)
{
	setContentType(response);
	std::vector<std::string> env = 
	{
		"REDIRECT_STATUS=200",
		"GATEWAY_INTERFACE=CGI/1.1",
		"PATH=/usr/bin:/bin",
		"REQUEST_METHOD=" + response->request->method,
		"SERVER_PROTOCOL=HTTP1.1",
		"SERVER_NAME=" + this->server_name,
		"SERVER_PORT=" + std::to_string(this->port),
		"CONTENT_TYPE=" + response->content_type,
		"SCRIPT_NAME=" + response->request->uri,
		"SCRIPT_FILENAME=" + response->request->uri,
		"PATH_INFO=" + response->request->uri,
		"CONTENT_LENGTH=" + std::to_string(body_size)
	};

	try {
		char**	envArray = new char*[env.size() + 1]{};
		for (uint32_t i = 0; i < env.size(); i++)
		{
			envArray[i] = new char[env[i].length() + 1];
			std::strcpy(envArray[i], env[i].c_str());
		}
		return (const char**)envArray;
	} catch (std::exception& e)
	{
		logToFile("initEnv - ERROR in creating enviroment variables: " + (std::string)e.what());
		return (nullptr);
	}
}

std::string server::readChildProcessOutPipe(int pipe_out)
{
	char buffer[1024];
	ssize_t bytesRead;
	std::string content;

	bytesRead = read(pipe_out, buffer, sizeof(buffer));
	if (bytesRead < 0) {
		logToFile("readChildProcessOutPipe - initial read FAILED from out pipe, sending 500");
		return ("");
	} else if (bytesRead == 0) {
		logToFile("readChildProcessOutPipe - initial read returned EOF");
		return ("");
	}

	content.append(buffer, bytesRead);

	while ((bytesRead = read(pipe_out, buffer, sizeof(buffer))) > 0) {
		content.append(buffer, bytesRead);
	}
	
	if (bytesRead < 0) {
		logToFile("readChildProcessOutPipe - subsequent read FAILED, sending 500");
		return ("");
	}
	return (content);
}
