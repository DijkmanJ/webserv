/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   post.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: ohendrix <ohendrix@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/25 16:16:47 by ohendrix      #+#    #+#                 */
/*   Updated: 2024/12/19 12:36:55 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

void server::handlePost(t_response *response, std::string &req_str)
{
	std::string fileName = getFileName(req_str);
	std::string extension = fileName.substr(fileName.find_last_of('.') + 1);
	std::string dir_path = extractFolderFromUri(response->request->uri);

	if (postFileTypeNotAllowed(this->directories[dir_path].types, extension))
		return (setErrorPageResponse(response, 415));
	
	std::string body = getPostBody(req_str);
	int status = isInvalidRequest(body);
	if (status > 0)
		return (setErrorPageResponse(response, status));

	if (setFileToUploads(body, req_str, dir_path) == -1)
		return (setErrorPageResponse(response, 500));

	response->statuscode = S_201;
}

int postFileTypeNotAllowed(const std::string& allowed_types, const std::string& extension)
{
	if (!stringContains(allowed_types, extension))
		return (1);

	int pos = allowed_types.find(extension) + extension.length();
	if (allowed_types[pos] != ' ' && allowed_types[pos] != ',' && allowed_types[pos] != '\0')
		return (1);
	return (0);
}

std::string server::getPostBody(const std::string &req_str)
{
	std::string boundary = getBoundary(req_str);
	if (boundary == S_400)
	{
		logToFile("No Boundary in post file found");
		return (S_400);
	}

	size_t file_header_start = req_str.find("Content-Disposition: form-data;");
	if (file_header_start == std::string::npos)
	{
		logToFile("Incomplete File header Found");
		return (S_400);
	}

	size_t file_header_end = req_str.find("\r\n\r\n", file_header_start);
	if (file_header_end == std::string::npos)
	{
		logToFile("Incomplete File header Found - No EOF");
		return (S_400);
	}

	std::string content = req_str.substr(file_header_end + 4);
	content = content.substr(0, (content.size() - boundary.size() - 5));
	if (content == "\r\n")
	{
		logToFile("Incomplete File header Found - No EOF");
		return (S_400);
	}
	return (content);
}

std::string server::getBoundary(const std::string &req_str)
{
	std::string boundary;
	try {
		std::string boundary_tag = "boundary=";
		boundary = req_str.substr(req_str.find(boundary_tag) + boundary_tag.size());
		boundary = boundary.substr(0, boundary.find('\n'));
	} catch (std::exception& e) {
		std::cerr << "Error: No boundry found in request " << std::endl;
		logToFile("Error: No boundry found in request ");
		return S_400;
	}
	return (boundary);
}

std::string server::getFileName(const std::string &req_str)
{
	size_t pos = req_str.find("filename=\"");
	if (pos == std::string::npos)
		return "unknown";
	std::string filename = req_str.substr(pos + 10);
	filename = filename.substr(0, filename.find('\"'));
	return (filename);
} 

int server::setFileToUploads(const std::string &content, const std::string &req_str, const std::string &dir_path)
{
	std::ofstream outfile("." + dir_path + "/" + getFileName(req_str), std::ios::binary);
	if (!outfile.is_open())
	{
		logToFile("setFileToUploads - ERROR opening outfile");
		return (-1);
	}
	outfile.write(content.c_str(), content.size());
	outfile.close();
	return (0);
}