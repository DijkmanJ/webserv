/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   response_content.cpp                               :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/10 17:04:55 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/17 11:53:06 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

void server::setResponseContent(t_response *response)
{
	if (response->content_type.find("image") != std::string::npos)
		return (setImageContent(response));

	setPageContent(response);
}

void server::setImageContent(t_response *response)
{
	std::string path = "." + response->request->uri;

	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Failed to open image file: " << path << std::endl;
		logToFile("setImageContent - FAILED to open image file: " + path + ", sending 404");
		return (setErrorPageResponse(response, 404));
	}
	file.seekg(0, std::ios::end);
	int filesize = file.tellg();
	std::string body(filesize, '\0');
	file.seekg(0, std::ios::beg);
	file.read(&body[0], filesize);

	response->content = body;
}

void server::noErrorPageFound(t_response* response)
{
	response->content_type = "text/plain";
	response->content = response->statuscode;
	setContentLength(response);
}

void server::setPageContent(t_response *response, std::string given_path)
{
	std::string path = getPagePath(response, given_path);
	std::ifstream file(path.c_str());

	if (!file.is_open() || std::filesystem::is_directory(path))
	{
		if (path.substr(path.find_last_of('/') + 1) == this->index_file)
			setPageContent(response, "." + this->root + "/" + this->autoindex_page);
		else if (path == this->root + "/" + this->error_page)
			noErrorPageFound(response);
		else
			setErrorPageResponse(response, 404);
	}
	else
	{
		std::string line;
		while (std::getline(file, line))
			response->content += line + "\n";
	}
	if (file.is_open())
		file.close();
}

std::string server::getPagePath(t_response *response, std::string given_path)
{
	if (!given_path.empty())
		return (given_path);

	std::string	path = response->request->uri;
	if (path == "/")
		path = this->root + "/";

	if (isDirectory(path) && autoIndexIsOn(path))
		path = path + this->index_file;
	return("." + path);
}

int server::autoIndexIsOn(const std::string &path)
{
	std::string dir_path = path;
	if (dir_path.ends_with('/'))
		dir_path = dir_path.substr(0, dir_path.length() - 1);

	if (this->directories.find(dir_path) != directories.end())
	{
		if (this->directories[dir_path].autoindex == "on")
			return (1);
	}
	return 0;
}


