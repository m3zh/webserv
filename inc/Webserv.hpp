/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 15:08:47 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/05 09:12:11 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include "../inc/Config.hpp"
# include "../inc/Cgi.hpp"

class Config;
class Cgi;

class Webserv
{
    private:
        std::vector<ServerInfo> _servers;

    public:
        Webserv(std::vector<ServerInfo> &s);
        ~Webserv();

        std::vector<ServerInfo>&    getServers();

        // it could later be moved to the Request class
        bool                        isCGI_request(std::string http_content);            // will take a Request class paramas, check if action and method fit for cgi
};