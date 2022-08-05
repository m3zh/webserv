/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:42 by vmasse            #+#    #+#             */
/*   Updated: 2022/08/05 09:01:41 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <iostream>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <map>

struct CGIrequest
{
    std::string action;                         // action field in html 
    std::string method;                         // action field in html 
    std::string path_to_output;                 // upload_store in config
    std::string content_length;                 // content length field in HTML header
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
        void            exec_CGI();
        // void            child_process(CGIrequest& cgireq);
        // void            parent_process(CGIrequest& cgireq, int status);

        void            setEnv(char **env);
        void            set_CGIrequest(std::string action, std::string method, std::string content_length);
        void            clear_CGIrequest();                                             // reset CGIrequest fields to ""

        void            getEnv();                                                       // necessary ?
        std::string     getEnvValue(std::string key);                                   // necessary ?
        std::string     getFromQueryString(std::string var);            

        CGIrequest&     get_CGIrequest();
        std::string     get_CGIaction();
        std::string     get_CGImethod();
        std::string     get_CGIcontent_length();

        bool            is_GETmethod();

        void            http_header();
        void            redirect_http_header(std::string loc);
        void            cookies_http_header();
        void            session_http_header();                  // TO-DO
};


