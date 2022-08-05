/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:42 by vmasse            #+#    #+#             */
/*   Updated: 2022/08/05 22:36:11 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <vector>
# include <sys/wait.h>
# include <map>

# define READ 0
# define WRITE 1

// ****************************
// the CGI class execute dynamic web pages
// it supports GET and POST requests
// GET requests receive args from QUERY_STRING and passed them as args
// POST requests receive args from stdin
// ****************************

struct CGIrequest
{
    std::string action;                         // action field in html 
    std::string method;                         // method field in html 
    std::string path_to_output;                 // upload_store in config
    size_t      content_length;                 // content length field in HTML header
    int         socket_fd;                      // content length field in HTML header
};

class Cgi
{	
    private:
        std::map<std::string, std::string> _env;

        CGIrequest      _request;
        
        pid_t           _pid;
        int             _fds[2];
        
    public:

        Cgi(char **env);
        ~Cgi();

        void            parse_CGIrequest(std::string http_content);                     // parse the HTTP request 
        void            exec_CGI(CGIrequest& req);
        void            child_process(CGIrequest& req);
        void            parent_process(int status);

        void            setEnv(char **env);
        void            set_CGIrequest(std::string action, std::string method, size_t content_length); // size_t -> content length is never negative
        void            clear_CGIrequest();                                             // reset CGIrequest fields to ""

        void                        getEnv();                                                       // necessary ?
        std::string                 getEnvValue(std::string key);                                   // necessary ?
        std::vector<std::string>    getFromQueryString();            

        CGIrequest&     get_CGIrequest();
        std::string     get_CGIaction();
        std::string     get_CGImethod();
        size_t          get_CGIcontent_length();

        bool            is_GETmethod();
        bool            isCGI_request(std::string html_content);
        std::string     get_CGIscript(std::string action);

        void            http_header();
        void            redirect_http_header(std::string loc);
        void            cookies_http_header();
        void            session_http_header();                                          // TO-DO
};


