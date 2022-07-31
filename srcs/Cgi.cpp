/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:34 by mlazzare          #+#    #+#             */
/*   Updated: 2022/07/31 22:20:36 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Cgi.hpp"

Cgi::Cgi(char **env)        {     setEnv(env);    }
Cgi::~Cgi()                 {}

// void    Cgi::child_process()
// {
//     dup2(_fds[0], STDIN_FILENO);
//     dup2(_fds[1], STDOUT_FILENO);
//     close(_fds[0]);
// }

// void    Cgi::parent_process(int status)
// {
//     waitpid(-1, &status, 0);
//     close(_fds[0]);
//     close(_fds[1]);
// }

// void    Cgi::exec_cgi()
// {
//     int status = 0;
    
//     pipe(_fds);
//     _pid = fork();
//     if (_pid < 0)
//         exit(1);
//     if (_pid == 0)
//         child_process();
//     else
//         parent_process(status);
// }

void    Cgi::setEnv(char **env)
{
    int i = -1;

    while (env[++i])
    {
        std::string curr(env[i]);
        std::string key = curr.substr(0, curr.find("=") - 1);
        std::string value = curr.substr(curr.find("=") + 1, curr.size());
        _env.insert(std::make_pair(key, value));
    }
}

// ************
// GETTERS functions
// ************

std::string     Cgi::getEnvValue(std::string key)
{
    return _env[key];
}

void            Cgi::getEnv()                                           // print ENV for debugging purposes
{
    std::map<std::string, std::string>::iterator it = _env.begin();

    while (it != _env.end())
    {
        std::cout << it->first + " " + it->second << std::endl;
        it++;
    }
}

std::string            getFromQueryString(std::string var)
{
    std::string qs = getenv("QUERY_STRING");
    size_t pos = qs.find(var);
    if (pos != std::string::npos)
        return qs.substr(qs.find("=", pos+1) + 1, qs.find("&", pos+1));          // return var value, which is between '=' and '&'
    return 0;
}

// ************
// HTTP HEADERS functions
// ************

void    Cgi::http_header()
{
    std::cout << "Content-Type: text/html; charset=utf-8;\r\n\r\n";
}

void    Cgi::redirect_http_header(std::string loc)
{
    std::cout << "Status:308 Permanent redirect\nLocation: ";
    std::cout << loc;
    std::cout << "\r\n\r\n";
}

void    Cgi::cookies_http_header()
{
    std::cout << "Content-Type: text/html; charset=utf-8;\r\n";                 // getenv("HTTP_COOKIE")
    std::cout << "Set-Cookie: Hello Cookie World!\r\n\r\n";
}