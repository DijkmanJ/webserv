/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   serverManager.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/12 13:37:05 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/19 12:26:54 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/serverManager.hpp"
#include "../includes/webserv.hpp"
#include "../includes/server.hpp"


serverManager::serverManager(const char* configFile) 
	: serverCount(0), epoll_fd(epoll_create1(0))
{
	if (epoll_fd < 0)
		throw std::runtime_error("Failed to create epoll file descriptor");

	try {
		std::vector<std::string> sections = readAndSplitConfigFile(configFile);
		createServerList(sections);
	} catch (const std::exception& e){
		std::cerr << "Error: " << e.what() << " Exiting serverManager initialization" << std::endl;
		close(epoll_fd);
		throw;
	}

	while (serversAreRunning)
	{
		int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (event_count < 0) {
			if (errno == EINTR) {
				std::cerr << "epoll_wait interrupted by SIGINT" << std::endl;
				break;
			}
			throw std::runtime_error("Error during epoll wait");
		}
		for (int i = 0; i < event_count; i ++) {
			for (int j = 0; j < serverCount; j ++)
			{
				if (serverList[j] != nullptr)
					this->serverList[j]->runServer(events[i]);
				else
					std::cerr << "Warning: serverList[" << j << "] is null. Ignoring..." << std::endl;
			}
		}
	}
}

std::vector<std::string> serverManager::readAndSplitConfigFile(const char* configFile)
{
	std::string line;
	std::string content;
	std::ifstream file(configFile);
	
	if (!file)
		throw std::runtime_error("Config File not found");

	while (std::getline(file, line))
		content += line + "\n";

	std::vector<std::string> sections = extractSections(content);
	if (sections.empty())
		throw std::runtime_error("Invalid Config File: No sections {} found");

	return (sections);
}

std::vector<std::string> serverManager::extractSections(const std::string& text)
{
	std::vector<std::string> results;
	std::regex pattern(R"(\{([^}]*)\})");
	std::smatch matches;
	std::string temp = text;

	while (std::regex_search(temp, matches, pattern)) 
	{
		results.push_back(matches[1].str());
		temp = matches.suffix();
	}
	return results;
}

void serverManager::createServerList(std::vector<std::string> sections)
{
	std::vector<std::string>::iterator it;
	for (it = sections.begin(); it != sections.end(); ++it)
	{
		try {
			serverList.push_back(std::make_unique<server>(*it, epoll_fd));
			this->serverCount ++;
		} catch (const std::exception& e) {
			std::cerr << "Error: Failed to create server for section: " << *it
			<< ". Exception " << e.what() << std::endl;
		}
	}

	if (serverList.empty())
		throw std::runtime_error("No servers created from configfile");
}

serverManager::~serverManager()
{
	std::cout << "Destructor called for serverManager" << std::endl;
	if (epoll_fd > 0)
		close(epoll_fd);
}