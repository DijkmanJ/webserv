/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   serverManager.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/12 13:37:18 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/17 15:25:56 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class server;

class serverManager{
	public:
		serverManager(const char* configFile);
		~serverManager();
	private:
		int serverCount;
		int epoll_fd;
		struct epoll_event events[MAX_EVENTS];
		std::vector<std::unique_ptr<server>> serverList;
		std::vector<std::string> extractSections(const std::string& text);
		std::vector<std::string> readAndSplitConfigFile(const char* configFile);
		void createServerList(std::vector<std::string> sections);
};
