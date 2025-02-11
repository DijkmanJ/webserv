/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   delete.cpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: ohendrix <ohendrix@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/11/25 16:16:47 by ohendrix      #+#    #+#                 */
/*   Updated: 2024/12/17 13:41:35 by jdijkman      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server.hpp"
#include "../includes/serverManager.hpp"

void server::handleDelete(t_response *response)
{
	std::string path = "." + response->request->uri;
	std::filesystem::remove(path.c_str());
}