/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:42 by vmasse            #+#    #+#             */
/*   Updated: 2022/08/09 09:44:53 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <unistd.h>
# include <stdlib.h>
# include <string>
# include <string.h>
# include <vector>
# include <sys/wait.h>
# include <map>

# define READ 0
# define WRITE 1
# define MAX_SIZE 2000

// ****************************
// the CGI class execute dynamic web pages
// it supports GET and POST requests
// GET requests receive args from QUERY_STRING
// POST requests receive args from stdin
// ****************************

struct CGIrequest                               // would be cool if it could inherit from Request class
{
    std::string     action;                         // action field in html 
    std::string     method;                         // method field in html 
    std::string     path_to_script;                 // abs path to CGI script
    std::string     upload_store;                   // upload_store in config
    size_t          content_length;                 // content length field in HTML header
    int             socket_fd;                      // the CGI script output should be written to this fd
};

class Cgi
{	
    private:
        std::map<std::string, std::string> _env;

        CGIrequest      _request;
        pid_t           _pid;
        int             _fds[2];
        
        void            child_process(CGIrequest& req);
        void            parent_process(int status);

        void                        set_CGIenv(std::string html_content);                               // it should have two vars, http header and http body
        char**                      getEnv();                                                       // return env as a char** for execve
        std::string                 getEnvValue(std::string key);                                   
        std::vector<std::string>    getFromQueryString();            

        void            set_CGIrequest(std::string action, std::string method, size_t content_length); // size_t -> content length is never negative
        void            clear_CGIrequest();                                             // reset CGIrequest fields to ""
        
        std::string     get_CGIaction();
        std::string     get_CGImethod();
        size_t          get_CGIcontent_length();
        std::string     get_CGIscript(std::string action);

        bool            get_CGIparam(std::string param, std::string html_content, size_t &pos);
        std::string     set_CGIparam(std::string html_content, size_t &pos);

        // utils
        void            string2charstar(char **charstar, std::string str);

    public:

        Cgi();
        ~Cgi();

        void            parse_CGIrequest(std::string http_content);                     // parse the HTTP request 
        void            exec_CGI(CGIrequest& req);

        bool            isCGI_request(std::string html_content);
        CGIrequest&     get_CGIrequest();
        
        void            http_header();
        void            redirect_http_header(std::string loc);
};


