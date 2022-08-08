/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ablondel <ablondel@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 15:08:47 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/08 13:26:05 by ablondel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include "../inc/Config.hpp"
#define GREEN "\e[92m"
#define RED	"\033[31m"
#define RESET "\033[0m"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#define BACKLOG 255
#define NBPORTS 3
#define log(c, msg, x) std::cout << c << msg << x << "\n" << RESET;
# include "../inc/Cgi.hpp"

class Config;
class Cgi;

class Webserv: public ServerInfo
{
    private:
        std::vector<ServerInfo> _servers;
        std::vector<int> _ports;

    public:
        Webserv(std::vector<ServerInfo> &s);
        Webserv();
        ~Webserv();

        std::vector<ServerInfo>&    getServers();
        std::vector<int>&           getPorts();

        bool    isCGI_request(std::string html_content);            // check if action and method fit for cgi
        void    close_all(std::vector<int> &sockets);
        int     set_server(std::vector<int> &sockets, std::vector<struct sockaddr_in> &addrs);
        void    parse_request(std::string &request);
        int     run_server(std::vector<int> &sockets, std::vector<struct sockaddr_in> &addrs);
};