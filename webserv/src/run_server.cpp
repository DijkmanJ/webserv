/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   run_server.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/13 14:03:33 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/17 15:36:19 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

void server::runServer(epoll_event event)
{
	this->event = event;
	if (event.data.fd == server_fd)
	{
		acceptNewConnection(event.data.fd, epoll_fd);
	}
	if (client_server_fd_map.find(event.data.fd)->second == server_fd)
	{
		t_response *response;
		if (this->event.events & EPOLLIN)
		{
			response = handleRequest(event.data.fd);
			setEpollOut(event.data.fd, response);
		}
		if (this->event.events & EPOLLOUT)
		{
			std::string response_str = createResponseString(response);
			logToFile(response->statuscode);
			if (send(event.data.fd, response_str.c_str(), response_str.size(), 0) == -1)
				logToFile("ERROR sending response to client... closing connection");
			closeClientConnection();
			delete response->request;
			delete response;
		}
	}
}

void server::acceptNewConnection(int socket_fd, int epoll_fd)
{
	struct epoll_event new_client_ev;
	int client_fd = accept(socket_fd, NULL, NULL);
	if (client_fd == -1)
		logToFile("acceptNewConnection - ERROR accept failed");

	setNonBlocking(client_fd);
	new_client_ev.events = EPOLLIN;
	new_client_ev.data.fd = client_fd;
	client_server_fd_map[client_fd] = socket_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &new_client_ev) == -1)
	{
		logToFile("acceptNewConnection - ERROR in epoll_ctl, closing client connection");
		close(client_fd);
	}
}

void server::closeClientConnection()
{
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, NULL);
	close(event.data.fd);
	client_server_fd_map.erase(event.data.fd);
}

void server::setEpollOut(int client_fd, t_response *response)
{
	this->event.events = EPOLLOUT;
	this->event.data.fd = client_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &this->event) == -1)
	{
		delete response->request;
		delete response;
		logToFile("Epoll_ctl failed");
	}
}

std::string server::createResponseString(t_response *response)
{
	std::string response_str;
	response_str = "HTTP/1.1 " + response->statuscode + "\r\n"
					"Content-Type: " + response->content_type + "\r\n" 
					"Content-Length: " + response->content_length + "\r\n"
					"Connection: close\r\n\r\n"
					+ response->content;
	return (response_str);
}
