/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:34 by mlazzare          #+#    #+#             */
/*   Updated: 2022/07/31 15:35:31 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


Cgi::Cgi(unsigned char *buff, char **env) : _buffer(buff) 
                                            {     setEnv(env);    }
Cgi::~Cgi()                                 {}

void    Cgi::child_process()
{
    dup2(_fds[0], STDIN_FILENO);
    dup2(_fds[1], STDOUT_FILENO);
    close(_fds[0]);
}

void    Cgi::parent_process(int status)
{
    waitpid(-1, &status, 0);
    close(_fds[0]);
    close(_fds[1]);
}

void    Cgi::exec_cgi()
{
    int status = 0;
    
    pipe(_fds);
    _pid = fork();
    if (_pid < 0)
        exit(1);
    if (_pid == 0)
        child_process();
    else
        parent_process(status);
}

void    Cgi::setEnv(char **env)
{
    
}