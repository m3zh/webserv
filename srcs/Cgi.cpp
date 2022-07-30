/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:34 by mlazzare          #+#    #+#             */
/*   Updated: 2022/07/29 13:10:36 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


Cgi::Cgi(unsigned char *buff) : _buffer(buff) {}
Cgi::~Cgi() {}

void Cgi::child_process()
{
    dup2(_fds[0], STDIN_FILENO);
    dup2(_fds[1], STDOUT_FILENO);
    close(_fds[0]);
}

void Cgi::parent_process()
{
    int status = 0;

    waitpid(-1, &status, 0);
    close(_fds[0]);
    close(_fds[1]);
}

void Cgi::call_cgi()
{
    pipe(_fds);
    _pid = fork();
    if (_pid < 0)
        exit(1);
    else if (_pid == 0)
        child_process();
    parent_process();
}