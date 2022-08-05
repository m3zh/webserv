/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/31 16:09:14 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/05 17:38:50 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Webserv.hpp"

Webserv::Webserv()
{
    return ;
}

Webserv::Webserv(std::vector<ServerInfo> &s) : _servers(s)
{
    for (std::vector<ServerInfo>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        log(GREEN, "SERVER NAME = ", it->getServerName());
        log(GREEN, "SERVER PORT = ", it->getPort());
        log(GREEN, "MAX CLIENT BODY = ", it->getClientMaxBodySize());
        log(GREEN, "ROOT = ", it->getServerRoot());
        log(GREEN, "INDEX = ", it->getServerIndex());
        std::vector<page> current_pages = it->getPages();
        for (std::vector<page>::iterator it = current_pages.begin(); it != current_pages.end(); it++)
        {
            log(GREEN, "location = ", it->location_path);
            log(GREEN, "root directory = ", it->root);
            //log(GREEN, "index = ", it->index);
            log(GREEN, "upload path = ", it->upload_store);
            log(GREEN, "redirect = ", it->redirect);
            for (std::vector<std::string>::iterator rit = it->methods.begin(); rit != it->methods.end(); rit++)
                log(GREEN, "ALLOWED METHOD = ", *rit);
            log(GREEN, "autoindex = ", it->autoindex);
            log(GREEN, "CGI request = ", it->is_cgi);
        }
        log(RED, "----------------------------------------------------------", 0);
    }
    return ;
}

Webserv::~Webserv()
{
    return ;
}


// probably to refactor
bool        Webserv::isCGI_request(std::string html_content)
{
    std::string root = "/home/user42/webserv/cgi-bin/";                 // hardcoded here; this should be retrieved from ServerInfo > page > root
    size_t pos = html_content.find("action");
    if (pos == std::string::npos)
        {   std::cout << "Invalid action for CGI\n"; return false;              };
    pos += 7;                                                           // action="........", we want to start from the first \", ie pos + 7
    std::string action;
    while ( html_content[++pos] != '\0'
        && html_content[pos] != '\"')
        action += html_content[pos];
    size_t extension = action.size() - 3;
    if (action.compare(extension, action.size(), ".py")                 // check if it's a pyhton or perl script [ is this necessary ? ]
        && action.compare(extension, action.size(), ".pl"))
        {   std::cout << "Invalid file extension for CGI\n"; return false;      };
    pos = html_content.find("method");
    if (pos == std::string::npos)
        {   std::cout << "Invalid method for CGI\n"; return false;              };
    pos += 7;                                                           // method="........", we want to start from the first \", ie pos + 7
    std::string method;
    while ( html_content[++pos] != '\0'
        && html_content[pos] != '\"')
        method += html_content[pos];
    if (method.compare("get") != 0                                      // only method get and post are accepted for cgi
        && method.compare("post") != 0)
        {   std::cout << "Invalid method for CGI\n"; return false;              };
    root += action;
    if (access(root.c_str(), X_OK) < 0)                                 // if executable exist and it's executable
        {   std::cout << "File not executable by CGI\n"; return false;          };
    return true;
}

void    Webserv::close_all(std::vector<int> &sockets)
{
    for (std::vector<int>::iterator it = sockets.begin(); it != sockets.end(); it++)
        close(*(it));    
}

int     Webserv::set_server(std::vector<int> &sockets, std::vector<int> &ports, std::vector<struct sockaddr_in> &addrs)
{
    int on = 1;
    std::vector<int>::iterator socket_it = sockets.begin();
    std::vector<int>::iterator ports_it = ports.begin();
    std::vector<struct sockaddr_in>::iterator addr_it = addrs.begin();
    for (; socket_it != sockets.end(); ++socket_it, ++addr_it, ++ports_it)
	{
        if ((*socket_it = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            close_all(sockets);
			return -1;
        }
		if (fcntl(*socket_it, F_SETFL, O_NONBLOCK) < 0)
        {
            close_all(sockets);
			return -2;
        }
		if (setsockopt(*socket_it, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        {
            close_all(sockets);
			return -3;
        }
		memset(&*addr_it, 0, sizeof(*addr_it));
	    addr_it->sin_family = AF_INET;
	    addr_it->sin_addr.s_addr = inet_addr("127.0.0.1");
	    addr_it->sin_port = htons(*ports_it);
		if ((bind(*socket_it, (struct sockaddr *)&*addr_it, sizeof(*addr_it))) < 0)
        {
            close_all(sockets);
			return -4;
        }
		if ((listen(*socket_it, BACKLOG) < 0))
        {
            close_all(sockets);
			return -5;
        }
	}
	return 0;
}

void    Webserv::parse_request(std::string &request)
{
    size_t pos = 0;
    std::string res;
    if (!request.length())
        return ;
    while ((pos = request.find('\n')) != std::string::npos)
    {
        res = request.substr(0, pos);
        request.erase(0, pos + 1);
        std::cout << "|" << res << "|" << std::endl;
    }
}

int     Webserv::run_server(std::vector<int> &sockets, std::vector<int> &ports, std::vector<struct sockaddr_in> &addrs)
{
    std::vector<int> clients;
    struct timeval timeout;
	char buffer[1024];
	int end_server;
    int close_conn;
	int rc;
    int max;
    /////////////////////////////////////DATA TESTS
    std::string ok = "HTTP/1.1 200\r\n\r\n";
    std::string index;
    std::ifstream ifs;
    ifs.open("/Users/ablondel/Desktop/webserv/pages/website1/team.html", std::fstream::in);
    while(ifs.read(buffer, sizeof(buffer)))
        index.append(buffer, sizeof(buffer));
    index.append(buffer, ifs.gcount());
    ok.append(index);
    /////////////////////////////////////////
    end_server = false;
    rc = set_server(sockets, ports, addrs);
    if (rc < 0)
    {    return -1;}
	fd_set current_sockets;
	fd_set read_sockets;
    FD_ZERO(&current_sockets);
    max = sockets.back();
    for (std::vector<int>::iterator it = sockets.begin(); it != sockets.end(); it++)
        FD_SET(*it, &current_sockets);
    while (end_server == false)
    {
        FD_ZERO(&read_sockets);
        timeout.tv_usec = 0;
        timeout.tv_sec = 3 * 60;
        read_sockets = current_sockets;
        rc = select(max + 1, &read_sockets, NULL, NULL, &timeout);
        if (rc < 0)
        {
            close_all(sockets);
            end_server = true;
            break ;
        }
        if (rc == 0)
        {
            close_all(sockets);
            end_server = true;
            break ;
        }
        int rd = 0;
        int rw = 0;
        close_conn = false;
        for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); it++)
        {
			bzero(&buffer, sizeof(buffer)); /* Clear the buffer */
            rd = recv(*it, buffer, sizeof(buffer), 0);
            buffer[rd] = 0;
            std::string request(buffer);
            if (request.length() > 0)
			    printf("\x1B[32m[[DATA RECEIVED]]\x1B[0m\n\n%s", request.c_str());
            parse_request(request);
            request.clear();
			if (rd < 0)
			{
				break ;
			}
			if (rd == 0)
			{
				break ;
			}
            rw = send(*it, ok.c_str(), ok.size(), 0);
			if (rw < 0)
			{
				break ;
			}
			if (rw == 0)
			{
				break ;
			}
            close(*it);
			FD_CLR(*it, &current_sockets);
            clients.erase(it);
        }
        for (int i = 0; i < max; i++)
        {
            if (FD_ISSET(sockets[i], &read_sockets))
            {
                socklen_t len = sizeof(sockaddr[i]);
                int connection = accept(sockets[i], (struct sockaddr*)&addrs[i], &len);
                if (connection < 0)
                {
                    close_all(sockets);
                    end_server = true;
                }
                FD_SET(connection, &current_sockets);
                clients.push_back(connection);
                if (connection > max)
                    max = connection;
                break ;
            }
        }
    }
    close_all(sockets);
    (void)close_conn;
    return 0;
}
std::vector<ServerInfo>&     Webserv::getServers()       {   return _servers;    };
