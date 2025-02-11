/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   launch_server.cpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: ohendrix <ohendrix@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/19 15:54:45 by ohendrix      #+#    #+#                 */
/*   Updated: 2024/12/17 13:24:13 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"


void server::launchServer(void)
{
	if (initServerSocket() == -1) 
		logToFile("Error in initializing server socket");
	if (listen(server_fd, 3) == -1) 
		logToFile("Error in listening to client connections");

	struct epoll_event ev;
	ev = createEpollEvent(server_fd);

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1)
		logToFile("Error in adding server_fd to epoll");
}

sockaddr_in server::initServerAddress(void)
{
	struct addrinfo hints, *res;
	struct sockaddr_in address;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;
	if (getaddrinfo(this->host.c_str(), NULL, &hints, &res) != 0) {
		logToFile("Error resolving address");
		return sockaddr_in();
	}

	if (res == nullptr) {
		logToFile("Error: no address information returned");
		return sockaddr_in();
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
	address.sin_port = htons(this->port);

	freeaddrinfo(res);
	return address;
}

int server::initServerSocket()
{
	int opt = 1;
	this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
		return (-1);
	this->address = initServerAddress();
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		logToFile("Setsockopt FAILED");
		return (-1);
	}
	if (bind(server_fd, (sockaddr*)&this->address, sizeof(this->address)) < 0){
		logToFile("bind FAILED");
		return (-1);
	}
	setNonBlocking(server_fd);
	return (server_fd);
}

void setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

epoll_event createEpollEvent(int server_fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = server_fd;
	return (ev);
}
