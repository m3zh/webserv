/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:34 by mlazzare          #+#    #+#             */
/*   Updated: 2022/08/05 23:08:34 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Cgi.hpp"

Cgi::Cgi(char **env)        {     setEnv(env);    }
Cgi::~Cgi()                 {}

// probably to refactor
// probably to be moved elsewhere
// params probably to be changed or expanded
// checks action, method, content-length in html message 
// returns true if it's good for cgi and set CGI request if so
bool        Cgi::isCGI_request(std::string html_content)
{
    std::string root = "/home/user42/webserv/cgi-bin/";                 // hardcoded here; this should be retrieved from ServerInfo > page > root
    size_t pos = html_content.find("action");
    if (pos == std::string::npos)
        {   std::cout << "Invalid action for CGI\n"; return false;              };
    pos += 7;                                                           // action="........", we want to start from the first \", ie pos + 7
    std::string action;
    while ( html_content[++pos] != '\0'
        && html_content[pos] != '\"')
        action += html_content[pos];
    size_t extension = action.size() - 3;
    if (action.compare(extension, action.size(), ".py")                 // check if it's a pyhton or perl script [ is this necessary ? ]
        && action.compare(extension, action.size(), ".pl"))
        {   std::cout << "Invalid file extension for CGI\n"; return false;      };
    pos = html_content.find("method");
    if (pos == std::string::npos)
        {   std::cout << "No method for CGI\n"; return false;                    };
    pos += 7;                                                           // method="........", we want to start from the first \", ie pos + 7
    std::string method;
    while ( html_content[++pos] != '\0'
        && html_content[pos] != '\"')
        method += html_content[pos];
    if (method.compare("get") != 0                                      // only methods get and post are accepted for cgi
        && method.compare("post") != 0)
        {   std::cout << "Invalid method for CGI\n"; return false;              };
    size_t content_length = 0;
    // if (getenv("CONTENT_LENGTH"))                                    // TO BE HANDLED for POST requests
    //     content_length = atoi(getenv("CONTENT_LENGTH"));
    // if (!content_length)
    //     {   std::cout << "No content length for CGI\n"; return false;            };
    root += action;
    if (access(root.c_str(), X_OK) < 0)                                 // if executable exists and it's executable
        {   std::cout << "File not executable by CGI\n"; return false;  };
    set_CGIrequest(action, method, content_length);          
    return true;
}

void    Cgi::child_process(CGIrequest& req)
{
    std::cerr << "CHILD\n";
    
    char    **cmd = { 0 };
    cmd[0] = const_cast<char*>(get_CGIscript(req.action).c_str());      // determine if script is python3 or perl
    cmd[1] = const_cast<char*>(get_CGIaction().c_str());
    close(_fds[WRITE]);
    if (dup2(_fds[READ], STDIN_FILENO) < 0)
        {    perror("cgi dup2 in: "); exit(EXIT_FAILURE);  }    
    // if (req.socket_fd)
    //  if (dup2(req.socket_fd, STDOUT_FILENO) < 0)                        // redirect output to socket fd
    //     {    perror("cgi dup2 out: "); exit(EXIT_FAILURE);  }
    if (get_CGImethod() == "get")                                       // GET method
    {
        std::cerr << "CGI is executing GET request\n";
        if (getenv("QUERY_STRING"))
            std::vector<std::string> vals = getFromQueryString();
        size_t pos = 1;
        std::vector<std::string>::iterator it = vals.begin();
        while ( it != vals.end())
        {    cmd[++pos] = const_cast<char*>((*it).c_str()); it++;   }
        cmd[pos] = 0;
        execve(cmd[0], cmd, envp);
    }
    else                                                                 // POST method
    {
        std::cerr << "CGI is executing POST request\n";
    
    }    
}

void    Cgi::parent_process(int status)
{    
    close(_fds[READ]);
    waitpid(_pid, &status, 0);
    std::cerr << "PARENT\n";
}

void    Cgi::exec_CGI(CGIrequest& req)
{
    int status = 0;
    
    if (pipe(_fds) < 0)
    {    perror("cgi pipe:"); exit(EXIT_FAILURE);  }
    _pid = fork();
    if (_pid < 0)
    {    perror("cgi fork:"); exit(EXIT_FAILURE);  }
    if (_pid == 0)
        child_process(req);
    else
        parent_process(status);
}

// ************
// SETTERS functions ( + UNSETTERS )
// ************

void    Cgi::setEnv(char **env)
{
    int i = -1;

    while (env[++i])
    {
        std::string curr(env[i]);
        std::string key = curr.substr(0, curr.find("=") - 1);
        std::string value = curr.substr(curr.find("=") + 1, curr.size());
        _env.insert(std::make_pair(key, value));
    }
}

void    Cgi::set_CGIrequest(std::string action, std::string method, size_t content_length)
{
    _request.action = action;
    _request.method = method;
    _request.content_length = content_length;
};

void    Cgi::clear_CGIrequest()
{
    _request.action = "";
    _request.method = "";
    _request.content_length = 0;    
};

// ************
// GETTERS functions
// ************

std::string     Cgi::getEnvValue(std::string key)       {       return _env[key];       };

void            Cgi::getEnv()                                           // print ENV for debugging purposes
{
    std::map<std::string, std::string>::iterator it = _env.begin();

    while (it != _env.end())
    {
        std::cout << it->first + " " + it->second << std::endl;
        it++;
    }
}

std::vector<std::string>     Cgi::getFromQueryString()
{
    std::string qs = getenv("QUERY_STRING");
    std::vector<std::string> ret;
    size_t pos, start = 0;
    while (pos != std::string::npos)
    {
        pos = qs.find("=", start);
        start = pos;
        while (++start != std::string::npos && qs[start] != '&');
        if (start != std::string::npos)
            ret.push_back(qs.substr(pos + 1, start));                                // add var value, which is between '=' and '&'
    }        
    return ret;
}

CGIrequest&     Cgi::get_CGIrequest()                    {   return _request;    }
std::string     Cgi::get_CGIaction()                     {   return get_CGIrequest().action;    }
std::string     Cgi::get_CGImethod()                     {   return get_CGIrequest().method;    }
size_t          Cgi::get_CGIcontent_length()             {   return get_CGIrequest().content_length;    }

bool            Cgi::is_GETmethod()                      {   return strcmp(getenv("METHOD_REQUEST"),"get") == 0;    }
std::string     Cgi::get_CGIscript(std::string action)   {   if (action[action.size() - 1] == 'y')  return "python3";   return "perl";  }

// ************
// HTTP HEADERS functions
// ************

void    Cgi::http_header()
{
    std::cout << "Content-Type: text/html; charset=utf-8;\r\n\r\n";
}

void    Cgi::redirect_http_header(std::string loc)
{
    std::cout << "Status:308 Permanent redirect\nLocation: ";
    std::cout << loc;
    std::cout << "\r\n\r\n";
}

void    Cgi::cookies_http_header()
{
    std::cout << "Content-Type: text/html; charset=utf-8;\r\n";                 // getenv("HTTP_COOKIE")
    std::cout << "Set-Cookie: Hello Cookie World!\r\n\r\n";
}