/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   webserv.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/13 14:08:06 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/18 17:28:29 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# define MAX_EVENTS 10
# define TIME_OUT 3

// 2xx Success
#define S_200 "200 OK"
#define S_201 "201 Created"
#define S_204 "204 No Content"

// 3xx Redirects
#define S_301 "301 Moved Permanently"
#define S_302 "302 Found"
#define S_303 "303 See Other"
#define S_307 "307 Temporary Redirect"
#define S_308 "308 Permanent Redirect"

// 4xx Client Errors
#define S_400 "400 Bad Request"
#define S_401 "401 Unauthorized"
#define S_403 "403 Forbidden"
#define S_404 "404 Not Found"
#define S_405 "405 Method Not Allowed"
#define S_408 "408 Request Timeout"
#define S_413 "413 Payload Too Large"
#define S_414 "414 URI Too Long"
#define S_415 "415 Unsupported Media Type"
#define S_429 "429 Too Many Requests"

// 5xx Server Errors
#define S_500 "500 Internal Server Error"
#define S_501 "501 Not Implemented"
#define S_502 "502 Bad Gateway"
#define S_503 "503 Service Unavailable"
#define S_504 "504 Gateway Timeout"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define GREEN   "\033[32m"

//Basis Input/Output en Standaard Bibliotheken:
#include <iostream>		// Voor std::cout, std::cerr, etc.
#include <fstream>
#include <cstdlib>		// Voor functies zoals std::exit
#include <cstring>		// Voor C-string functies zoals strerror
#include <filesystem>
#include "json.hpp"

//Systeemfuncties en POSIX Functies:
#include <unistd.h>		// Voor fork, pipe, close, read, write, dup, dup2
#include <sys/types.h>	// Voor verschillende datatypes
#include <sys/stat.h>	// Voor file status functies (stat, open)
#include <fcntl.h>		// Voor file control opties (fcntl, open)

// Sockets en Netwerkcommunicatie:
#include <sys/socket.h>	// Voor socket, bind, listen, accept, send, recv
#include <netinet/in.h>	// Voor sockaddr_in, htons, htonl, ntohs, ntohl
#include <arpa/inet.h>	// Voor inet_pton, inet_ntop, etc.
#include <netdb.h>		// Voor getaddrinfo, freeaddrinfo, gai_strerror

//Sockets en Netwerkcommunicatie:
#include <sys/wait.h>	// Voor waitpid, om op processen te wachten
#include <signal.h>		// Voor signal handling

// Bestand- en Directorybeheer:
#include <dirent.h>		// Voor opendir, readdir, closedir
#include <sys/stat.h>	// Voor access en stat

// Voor epoll en kqueue:
#include <sys/epoll.h>	// Voor epoll_create, epoll_ctl, epoll_wait
// #include <sys/event.h>	// Voor kqueue, kevent (BSD-specifiek)

// Overige Standaard Functies:
#include <errno.h>		// Voor errno en foutmeldingen
#include <string.h>		// Voor string manipulatie
#include <map>			// Voor map
#include <unordered_map> // Voor unordered_map
#include <sstream>		// Voor std::istringstream
#include <regex>
#include <memory>
#include <exception>
#include <stdbool.h>

extern bool serversAreRunning;

typedef struct t_request
{
	std::string method;
	std::string uri;
} t_request;

typedef struct t_response
{
	std::string content_type;
	std::string content_length;
	std::string content;
	std::string statuscode;

	t_request *request;

} t_response;

int				stringContains(std::string haystack, std::string needle);
int				isDirectory(const std::string& path);
void			initRequestVariables(t_response *response, t_request *request, const std::string &req_str);

//Launch_server
epoll_event		createEpollEvent(int server_fd);
void			setNonBlocking(int fd);

//run_server

//post
int postFileTypeNotAllowed(const std::string& allowed_types, const std::string& extension);