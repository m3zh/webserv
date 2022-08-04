/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 15:08:47 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/04 11:19:01 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include "../inc/Config.hpp"

class Config;

class Webserv
{
    private:
        std::vector<ServerInfo> _servers;

    public:
        Webserv(std::vector<ServerInfo> &s);
        ~Webserv();

        std::vector<ServerInfo>&    getServers();

        bool                        isCGI_request();            // will take a Request class paramas, check if action and method fit for cgi
};