/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get.cpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: ohendrix <ohendrix@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/25 16:16:47 by ohendrix      #+#    #+#                 */
/*   Updated: 2024/12/17 11:04:46 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

void server::handleGet(t_response *response)
{
	if (response->request->uri.find(this->tag_autoindex) != std::string::npos)
		return listAutoIndex(response);
	setContentType(response);
	setResponseContent(response);
	setContentLength(response);
}

void server::listAutoIndex(t_response *response)
{
	std::string uploadpath = "." + extractFolderFromUri(response->request->uri);
	if (!std::filesystem::exists(uploadpath))
	{
		logToFile("listAutoIndex - path does not exist: " + uploadpath);
		setErrorPageResponse(response, 500);
	}
	std::string jsonfiles = listFilesInDirectory(uploadpath);
	if (jsonfiles.empty())
		return (setErrorPageResponse(response, 404));
	response->content = jsonfiles;
	response->content_type = "application/json";
	response->content_length = std::to_string(response->content.length());
}

std::string server::listFilesInDirectory(std::string path) 
{
	std::vector<std::string> files;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_regular_file())
			files.push_back(entry.path().filename().string());
	}
	nlohmann::json jsonFiles = files;
	return jsonFiles.dump();
}



