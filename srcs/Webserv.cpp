/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/04 11:10:36 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Webserv.hpp"

Webserv::Webserv(std::vector<ServerInfo> &s)  : _servers(s) 
                                                {}
Webserv::~Webserv()                             {}

std::vector<ServerInfo>&     Webserv::getServers()       {   return _servers;    };

bool                         Webserv::isCGI_request()
{
    return true;
}