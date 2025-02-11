/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   utils.cpp                                          :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/10 17:01:19 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/19 12:37:37 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

int stringContains(std::string haystack, std::string needle)
{
	if (haystack.find(needle) != std::string::npos)
		return (1);
	return (0);
}

void initRequestVariables(t_response *response, t_request *request, const std::string &req_str)
{
	request->method = req_str.substr(0, req_str.find(' '));
	request->uri = req_str.substr(req_str.find(' ') + 1);
	request->uri = request->uri.substr(0, request->uri.find(' '));
	response->statuscode = S_200;
	response->content_type = "text/html";
	response->request = request;
	response->request->uri = std::filesystem::path(response->request->uri).lexically_normal();
}

int isDirectory(const std::string& path)
{
	if (!path.ends_with('/'))
		return (0);
	if (std::filesystem::exists(path.substr(1)) && std::filesystem::is_directory(path.substr(1)))
		return (1);
	return (0);
}

void server::setContentType(t_response *response)
{
	if (response->request->uri == "/")
		return ;
	std::string extension = response->request->uri.substr(response->request->uri.find_last_of('.') + 1);
	
	auto it = this->FILE_TYPES.find(extension);
	if (it != this->FILE_TYPES.end())
		response->content_type = it->second; 
	else
		response->content_type = this->FILE_TYPES.at("default");

	try 
	{
		nlohmann::json parsed_data = nlohmann::json::parse(response->content);
		response->content_type = "application/json";
	} 
	catch (const nlohmann::json::parse_error& e) {}
}

void server::setContentLength(t_response *response)
{
	response->content_length = std::to_string(response->content.size());
}

std::string server::extractFolderFromUri(const std::string& uri)
{
	if (uri == "/" || uri == this->root)
		return (this->root);
	std::string dir_path = uri.substr(0, uri.find_last_of('/'));
		
	return dir_path;
}