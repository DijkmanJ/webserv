/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   server.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/13 14:03:59 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/18 16:13:18 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

typedef struct s_dir{
	std::string 	path;
	std::string 	methods;
	std::string 	types;
	std::string		autoindex;
} t_dir;

class server 
{
	public:
		server(const std::string& serverConfigStr, int& epoll_fd);
		~server();
		void runServer(epoll_event event);

	private:
		int&				epoll_fd;
		int					server_fd;
		epoll_event			event;
		struct sockaddr_in	address;
		std::map<int, int>	client_server_fd_map;
		std::ofstream		logFile;

		std::string 		server_name;
		int 				port;
		std::string 		host;
		int 				client_max_body_size;

		std::string			error_page;
		std::string 		index_file;
		std::string 		cgi_path;
		std::string 		tag_autoindex;
		std::string 		autoindex_page;
		std::string 		root;

		std::map<std::string, t_dir> directories;

		//defaults
		static const std::map<std::string, std::string>		DEFAULTS;
		static const std::map<int, std::string>				ERROR_CODES;
		static const std::map<std::string, std::string>		FILE_TYPES;
		static const int 									DEFAULT_PORT = 8080;
		static const int									DEFAULT_CLIENT_MAX_BODY_SIZE = 1048576;

		//parse_server.cpp
		int							parseServer(std::string config);
		std::vector<std::string> 	extractDirs(const std::string& text);
		std::string 				getConfigContent(std::string keyWord, std::string config);
		t_dir						createDirStruct(const std::string& config);

		//launch_server.cpp
		void			launchServer(void);
		sockaddr_in		initServerAddress(void);
		int				initServerSocket(void);

		//run_server.cpp
		void			acceptNewConnection(int server_fd, int epoll_fd);
		void			closeClientConnection();
		void			setEpollOut(int client_fd, t_response *response);
		std::string		createResponseString(t_response *response);

		//handle_request
		t_response*		handleRequest(int client_fd);
		std::string		readRequest(int client_fd);
		int				isInvalidRequest(const std::string& req_str);
		t_response*		handleBadRequest(t_response *response, t_request *request, const std::string &req_str);
		int				uriIsAllowedDirectory(const std::string &uri);
		int				methodIsAllowed(const std::string &method, const std::string &uri);
		int				getContentLength(const std::string& request);

		//cgi.cpp
		void 			handleCGI(t_response *response, const std::string& req_str);
		const char**	initEnv(t_response *response, int body_size);
		std::string		readChildProcessOutPipe(int pipe_out);
		std::string		getRequestBody(const std::string& req_str);

		//delete
		void			handleDelete(t_response *response);

		//post
		void			handlePost(t_response *response, std::string &req_str);
		std::string		getPostBody(const std::string &req_str);
		std::string		getBoundary(const std::string &req_str);
		std::string		getFileName(const std::string &req_str);
		int				setFileToUploads(const std::string &content, const std::string &req_str, const std::string& dir_path);

		//get
		void			handleGet(t_response *response);
		void			listAutoIndex(t_response *response);
		std::string		listFilesInDirectory(std::string path); 

		//response_content
		void			setResponseContent(t_response *response);
		void			setImageContent(t_response *response);
		void			setPageContent(t_response *response, std::string given_path = "");
		void			noErrorPageFound(t_response* response);
		std::string		getPagePath(t_response *response, std::string given_path = "");
		int				autoIndexIsOn(const std::string& path);

		//server_utils
		void			logToFile(const std::string& message);

		//error_page
		void			setErrorPageResponse(t_response *response, int err_code);
		void			replaceErrorVariables(std::string& content, int err_code, std::string err_msg);
		
		//utils
		void			setContentType(t_response *response);
		void			setContentLength(t_response *response);
		std::string		extractFolderFromUri(const std::string& uri);
};