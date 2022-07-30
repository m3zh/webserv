/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:42 by vmasse            #+#    #+#             */
/*   Updated: 2022/07/29 13:13:00 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>

class Cgi
{
    private:

        unsigned char   *_buffer;
        pid_t           _pid;
        int             _fds[2];

    public:

        Cgi(unsigned char *buff);
        ~Cgi();

        void            http_header();
        void            call_cgi();
        void            child_process();
        void            parent_process();
};


