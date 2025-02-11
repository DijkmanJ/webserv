/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   handle_request.cpp                                 :+:    :+:            */
/*                                                     +:+                    */
/*   By: ohendrix <ohendrix@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/25 16:16:47 by ohendrix      #+#    #+#                 */
/*   Updated: 2024/12/19 12:35:59 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

t_response *server::handleRequest(int client_fd)
{
	t_request		*request = new t_request;
	t_response		*response = new t_response;
	
	std::string req_str = readRequest(client_fd);
	if (isInvalidRequest(req_str))
		return (handleBadRequest(response, request, req_str));

	initRequestVariables(response, request, req_str);
	logToFile(request->method + " " + request->uri);

	if (!uriIsAllowedDirectory(request->uri))
		setErrorPageResponse(response, 404);
	else if (!methodIsAllowed(request->method, request->uri))
		setErrorPageResponse(response, 405);
	else if (stringContains(response->request->uri, this->cgi_path))
		handleCGI(response, req_str);
	else if (request->method == "GET")
		handleGet(response);
	else if (request->method == "POST")
		handlePost(response, req_str);
	else if (request->method == "DELETE")
		handleDelete(response);
	return (response);
}

int server::isInvalidRequest(const std::string& req_str)
{
	size_t pos = req_str.find(' ');
	if (pos == std::string::npos)
	{
		logToFile("No Space found in isInvalidRequest" + req_str);
		return (-1);
	}

	std::string code_str = req_str.substr(0, pos);
	std::string message_str = req_str.substr(pos + 1);

	try {
		int code = std::stoi(code_str);
		auto it = this->ERROR_CODES.find(code);
		if (it != this->ERROR_CODES.end() && it->second == message_str) {
			logToFile("ReadRequest - invalid Request found: " + req_str);
			return code;
		}
	} catch (std::exception& e) {
		return (0);
	}
	return (0);
}

t_response *server::handleBadRequest(t_response *response, t_request *request, const std::string &req_str)
{
	response->request = request;
	request->method = "GET";
	setErrorPageResponse(response, isInvalidRequest(req_str));
	logToFile("handleBadRequest called");
	return (response);
}

int server::uriIsAllowedDirectory(const std::string &uri)
{
	std::string dir_path = extractFolderFromUri(uri);
	if (this->directories.find(dir_path) == directories.end())
		return (0);
	if (!std::filesystem::exists(dir_path.substr(1)))
		return (0);
	return (1);
}

int	server::methodIsAllowed(const std::string &method, const std::string &uri)
{
	std::string dir_path = extractFolderFromUri(uri);

	if (dir_path.empty())
		dir_path = this->root;
	if (method != "GET" && method != "POST" && method != "DELETE")
		return (0);
	if (!stringContains(this->directories[dir_path].methods, method))
		return (0);
	return (1);
}

int server::getContentLength(const std::string& request)
{
	int content_length = 0;

	size_t pos = request.find("Content-Length: ");
	if (pos != std::string::npos) {
		size_t start_pos = pos + 16;
		size_t end_pos = request.find("\r\n", start_pos);

		if (end_pos == std::string::npos) {
			end_pos = request.length();
		}

		std::string length_str = request.substr(start_pos, end_pos - start_pos);
		try {
			content_length = std::stoi(length_str);
		} catch (std::exception& e) {
			logToFile("getContentLength - Invalid content-length: "
			 		+ length_str + " ERROR: " + (std::string)e.what());
			content_length = -1;
		}
	}
	return content_length;
}

std::string server::readRequest(int client_fd) 
{
	std::vector<char> buffer(8192); //implement max header size?
	std::string request;
	auto start_time = std::chrono::steady_clock::now();

	ssize_t bytes = read(client_fd, buffer.data(), buffer.size());
	if (bytes <= 0) //implement max header size?
	{
		logToFile("readRequest - ERROR when reading request header, sending 500");
		return (S_500);
	}
	request.append(buffer.data(), bytes);

	int content_length = getContentLength(request);
	if (content_length == -1)
		return (S_500);
	size_t total_length = (size_t)content_length + request.find("\r\n\r\n") + 4;
	
	while (request.size() < total_length) {
		auto current_time = std::chrono::steady_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
		if (elapsed_time.count() > 4)
		{
			logToFile("readRequest - ERROR timeout occured when reading client_fd");
			return (S_408);
		}
		bytes = read(client_fd, buffer.data(), buffer.size());
		if (bytes <= 0)
			continue;
		request.append(buffer.data(), bytes);
	}
	if ((int)content_length > this->client_max_body_size)
		return (S_413);
	if (read(client_fd, buffer.data(), buffer.size()) > 0) // if more data than declared is being send
	{
		logToFile("readRequest - request body larger than declared. Skipping request, sending 400");
		return (S_400);
	}
	if (request.length() > 0)
		return request;
	logToFile("readRequest - FAILED, sending 400");
	return (S_400);
}