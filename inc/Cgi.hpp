/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artmende <artmende@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:42 by vmasse            #+#    #+#             */
/*   Updated: 2022/10/10 17:16:14 by artmende         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <fstream>
# include <sstream>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string>
# include <string.h>
# include <vector>
# include <sys/wait.h>
# include <sys/stat.h>
# include <limits.h>
# include <map>

# include "Webserv.hpp"
# include "Client.hpp"
# include "Request.hpp"

# define READ 0
# define WRITE 1
# define MAX_SIZE 2000
# define CGI_PATH "cgi-bin"
# define TMPFILE  "tmp.XXXXXX"

// ****************************
// the CGI class execute dynamic web pages
// it supports GET and POST requests
// GET requests receive args from QUERY_STRING
// POST requests receive args from stdin
// ****************************

class Request;

struct CGIrequest
{
    std::string     action;                         // action field in html 
    std::string     method;                         // method field in html 
    std::string     path_to_script;                 // abs path to CGI script
    std::string     upload_store;                   // upload_store in config
    std::string     absolute_path;                  // upload_store in config
    size_t          content_length;                 // content length field in HTML header
};

class Cgi
{	
    private:
        std::map<std::string, std::string> _env;

        CGIrequest      _request;
        pid_t           _pid;
        int             _fds[2];
        FILE            *_stdin;
        FILE            *_stdout;
        
        void            child_process()                 const;
        void            parent_process(int status)      const;
        void            sendCGI_response(Client *c)      ;

        void                        set_CGIenv(Request const &req, std::map<std::string, std::string> header, ServerInfo *server);                                  // it should have two vars, http header and http body
        char**                      getEnv()                                            const;                                                                      // return env as a char** for execve
        std::string                 getEnvValue(std::string key)                        ;                                   
        std::string                 getFromQueryString(std::string uri)                 const;            

        void            set_CGIrequest(Request const & req, std::map<std::string, std::string> header,
                                        std::string path_to_script, ServerInfo *server); // size_t -> content length is never negative
        void            clear_CGIrequest();                                             // reset CGIrequest fields to ""
        
        std::string     get_CGIaction()                          ;
        std::string     get_CGImethod()                          ;
        size_t          get_CGIcontent_length()                  ;
        std::string     get_CGIscript(std::string action)   const;

        bool            get_CGIparam(std::string param, std::string html_content, size_t &pos);
        std::string     set_CGIparam(std::string html_content, size_t &pos);

        // utils
        void            string2charstar(char **charstar, std::string str)   const;
        std::string     file2string(int fd)                                 const;

    public:

        Cgi();
        ~Cgi();

        void            parse_CGIrequest(std::string http_content);                     // parse the HTTP request 
        void            exec_CGI(Request const & req, Client *c)    ;

        bool            isCGI_request(Client *c);
        CGIrequest&     get_CGIrequest();
};



