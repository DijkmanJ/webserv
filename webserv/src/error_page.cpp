/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   error_page.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: jdijkman <jdijkman@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/12/10 16:58:33 by jdijkman      #+#    #+#                 */
/*   Updated: 2024/12/19 12:35:40 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

void server::setErrorPageResponse(t_response *response, int err_code)
{
	std::string err_msg = ERROR_CODES.find(err_code)->second;
	response->statuscode = std::to_string(err_code) + ' ' + err_msg;
	response->content_type = "text/html";
	if (std::filesystem::exists((this->root + "/" + this->error_page).substr(1)))
	{
		setPageContent(response, "." + this->root + "/" + this->error_page);
		replaceErrorVariables(response->content, err_code, err_msg);
		setContentLength(response);
		return ;
	}
	response->content = response->statuscode;
}

void server::replaceErrorVariables(std::string& content, int err_code, std::string err_msg) 
{
	size_t pos;

	while ((pos = content.find("{{error_code}}")) != std::string::npos) {
		content.replace(pos, 14, std::to_string(err_code));
	}
	while ((pos = content.find("{{error_message}}")) != std::string::npos) {
		content.replace(pos, 17, err_msg);
	}
}