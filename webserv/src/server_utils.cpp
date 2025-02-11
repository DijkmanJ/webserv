/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server_utils.cpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/10 16:56:39 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/10 16:56:48 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

void server::logToFile(const std::string& message)
{
	if (logFile.is_open()) {
		logFile << message << std::endl;
	}
	else
		std::cerr << "Log file already closed." << std::endl;
}