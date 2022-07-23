# pragma once
# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>

class Cgi
{
    public:

        Cgi(unsigned char *buff);
        ~Cgi();

        void call_cgi();
        void child_process();
        void parent_process();

    private:

        unsigned char *_buffer;
        pid_t   _pid;
        int     _fds[2];
};

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
    {
        child_process();
    }
    parent_process();
}
