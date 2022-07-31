/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:42 by vmasse            #+#    #+#             */
/*   Updated: 2022/07/31 15:34:38 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>

class Cgi
{	
    private:
        std::map<std::string, std::string> env;

        unsigned char   *_buffer;
        pid_t           _pid;
        int             _readfds[2];
        int             _writefds[2];

    public:

        Cgi(unsigned char *buff, char **env);
        ~Cgi();

        void            http_header();
        void            exec_cgi();
        void            child_process();
        void            parent_process(int status);

        void            setEnv(char **env)
};


