/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:42 by vmasse            #+#    #+#             */
/*   Updated: 2022/08/03 11:14:24 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <iostream>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <map>

class Cgi
{	
    private:
        std::map<std::string, std::string> _env;

        // unsigned char   *_buffer;
        // pid_t           _pid;
        // int             _readfds[2];
        // int             _writefds[2];
        
    public:

        Cgi(char **env);
        ~Cgi();

        // void            exec_cgi();
        // void            child_process();
        // void            parent_process(int status);

        void            setEnv(char **env);
        std::string     getEnvValue(std::string key);
        void            getEnv();
        std::string     getFromQueryString(std::string var);

        bool            is_GETmethod();

        void            http_header();
        void            redirect_http_header(std::string loc);
        void            cookies_http_header();
        void            session_http_header();                  // TO-DO
};


