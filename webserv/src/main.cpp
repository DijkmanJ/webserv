/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   main.cpp                                           :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/08 17:25:13 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/19 12:36:31 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

bool serversAreRunning = true;

void sig_handler(int signal)
{
	if (signal == SIGINT || signal == SIGTSTP)
	{
		std::cerr << "\nSignal " << signal << " received. Shutting down..." << std::endl;
		serversAreRunning = false;
	}
}

int main(int argc, char **argv)
{
	signal(SIGINT, sig_handler);
	signal(SIGTSTP, sig_handler);

	const char* configFile = (argc < 2) ? "config/default_config" : argv[1];
	if (argc < 2)
		std::cout << "Launching server using default configfile: " << configFile << std::endl;

	try {
		serverManager manager(configFile);
	} catch (std::exception& e) {
		std::cerr << "Critical error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
