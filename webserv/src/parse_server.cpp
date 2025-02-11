/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   parse_server.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: ohendrix <ohendrix@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/19 15:50:23 by ohendrix      #+#    #+#                 */
/*   Updated: 2024/12/19 11:47:01 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

server::server(const std::string& serverConfigStr, int& epoll_fd)
	: epoll_fd(epoll_fd), server_fd(-1)
{
	parseServer(serverConfigStr);
	this->logFile.open("log_" + this->server_name + ".txt", std::ios::app);
	
	std::vector<std::string> dirConfigStr = extractDirs(serverConfigStr);
	for (const std::string& str : dirConfigStr) {
		std::string path = getConfigContent("PATH", str);
		this->directories[path] = createDirStruct(str);
	}
	
	if (!logFile.is_open()) {
		throw std::runtime_error("Failed to open log file for server: " + this->server_name);
	}
	logToFile("Server: " + this->server_name + " constructor called");
	launchServer();
}

server::~server()
{
	std::cout << "Destructor called for server"  << std::endl;
	if (logFile.is_open()) {
		logFile.close();
	}
	if (server_fd != -1)
		close(server_fd);
}

int server::parseServer(std::string config)
{
	try {
		this->port = std::stoi(getConfigContent("PORT", config));
		if (this->port < 1 || this->port > 65535)
			throw std::out_of_range("");
	} catch(const std::exception& e) {
		std::cerr << "Port out of range: " << e.what() << " - using default (8080)\n";
		this->port = 8080;
	}

	try {
		this->client_max_body_size = std::stoi(getConfigContent("CLIENT_MAX_BODY_SIZE", config));
	} catch (std::exception &e) {
		std::cerr << "CLIENT_MAX_BODY_SIZE  " << e.what() << " - using default (4096)" << std::endl;
		this->client_max_body_size = 4096;
	}

	this->server_name = getConfigContent("SERVER", config);
	this->host = getConfigContent("HOST", config);
	this->error_page = getConfigContent("ERROR_PAGE", config);
	this->index_file = getConfigContent("INDEX_FILE", config);
	this->cgi_path = getConfigContent("CGI_PATH", config);
	this->tag_autoindex = getConfigContent("TAG_AUTOINDEX", config);
	this->root = getConfigContent("ROOT", config);
	this->autoindex_page = getConfigContent("AUTOINDEX_PAGE", config);

	return 0;
}

std::vector<std::string> server::extractDirs(const std::string& text)
{
	std::vector<std::string> results;
	std::regex pattern(R"(\[([^\]]*)\])");
	std::smatch matches;
	std::string temp = text;

	while (std::regex_search(temp, matches, pattern)) 
	{
		results.push_back(matches[1].str());
		temp = matches.suffix();
	}
	return results;
}

t_dir server::createDirStruct(const std::string& config)
{
	t_dir dirConfig;
	dirConfig.path = getConfigContent("PATH", config);
	dirConfig.methods = getConfigContent("METHODS", config);
	dirConfig.autoindex = getConfigContent("AUTO_INDEX", config);
	dirConfig.types = getConfigContent("POST_ALLOWED_TYPES", config);
	return dirConfig;
}

std::string server::getConfigContent(std::string keyWord, std::string config)
{
	size_t pos = config.find(keyWord);
	if (pos == std::string::npos)
	{
		auto it = DEFAULTS.find(keyWord);
		if (it != DEFAULTS.end())
		{
			std::cerr << "No " + keyWord + " Found in config file - using default (" + it->second + ")" << std::endl;
			return (it->second);
		}
		else
		{
			std::cerr << "NO Default found in default list - leaving blank" << std::endl;
			return "";
		}
	}
	std::string line = config.substr(pos + keyWord.length());
	return (line.substr(line.find_first_not_of(" \t"), line.find('\n') - line.find_first_not_of(" \t")));
}

const std::map<std::string, std::string> server::DEFAULTS = {
	{"SERVER", "default_name"},
	{"PORT", "8080"},
	{"HOST", "0.0.0.0"},
	{"ERROR_PAGE", "error.html"},
	{"CLIENT_MAX_BODY_SIZE", "4096"},
	{"ALLOWED_METHODS", "GET"},
	{"INDEX_FILE", "index.html"},
	{"AUTO_INDEX", "off"},
	{"CGI_PATH", "/usr/bin/cgi"},
	{"TAG_AUTOINDEX", "/list-files"},
	{"POST_ALLOWED_TYPES", ""},
	{"AUTOINDEX_PAGE", "autoindex.html"},
	{"ROOT", "/www"}
};

const std::map<int, std::string> server::ERROR_CODES = {
	{200, "OK"},
	{201, "Created"},
	{204, "No Content"},

	// 3xx Redirects
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},

	// 4xx Client Errors
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{408, "Request Timeout"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{429, "Too Many Requests"},

	// 5xx Server Errors
	{500, "Internal Server Error"},
	{501, "Not Implemented"}, // put
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Timeout"}
};

const std::map<std::string, std::string> server::FILE_TYPES = {
	// Text Types
	{"html", "text/html"},
	{"css", "text/css"},
	{"js", "application/javascript"},
	{"json", "application/json"},
	{"txt", "text/plain"},
	{"xml", "application/xml"},
	// Image Types
	{"png", "image/png"},
	{"jpg", "image/jpeg"},
	{"jpeg", "image/jpeg"},
	{"gif", "image/gif"},
	{"webp", "image/webp"},
	{"svg", "image/svg+xml"},
	{"bmp", "image/bmp"},
	{"ico", "image/x-icon"},
	// Audio Types
	{"mp3", "audio/mpeg"},
	{"ogg", "audio/ogg"},
	// Video Types
	{"mp4", "video/mp4"},
	{"webm", "video/webm"},
	// Archive Types
	{"zip", "application/zip"},
	{"tar", "application/x-tar"},
	// Document Types
	{"pdf", "application/pdf"},
	// Default for unknown file types 
	{"default", "text/html"}
};