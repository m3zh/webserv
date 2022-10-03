/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:34 by mlazzare          #+#    #+#             */
/*   Updated: 2022/10/03 13:40:11 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Cgi.hpp"
# include <string.h>

Cgi::Cgi()                  {};
Cgi::~Cgi()                 {};

// checks action, method, content-length in html message 
// returns true if it's good for cgi
bool        Cgi::isCGI_request(Client *c)
{
    std::string         pwd(getenv("PWD"));
    std::string         path_to_script;
    std::string         upload_store;
    ServerInfo*         _server = c->getServerInfo();
    Request             req = c->getRequest();
    std::vector<page>   pages = _server->getPages();
    std::vector<page>::iterator cgi_page = pages.begin();
        
    for ( ; cgi_page != pages.end(); cgi_page++ )
    {                                                                     // check for cgi location in config
        if ( (*cgi_page).location_path == CGI_PATH )                                                
        {   path_to_script = pwd + (*cgi_page).root;                        // set path to script                                                      
            upload_store = (*cgi_page).upload_store;  break ;     }                                 // set path to upload_store
    }                                                      
    std::map<std::string, std::string> header = req.get_header_map();
    // ------
    // ACTION
    // ------
    std::cout << "LOC: ";
    std::cout << req.get_location() << std::endl;
    std::string action = req.get_location();
    size_t check4querystring = req.get_location().find("?");
    if ( check4querystring != std::string::npos)
        action = action.substr(0, req.get_location().find("?"));
    std::cout << "ACT: ";
    std::cout << action << std::endl;
    size_t extension = action.size() - 3;
    if (action.compare(extension, action.size(), ".py"))                       // check if it's a pyhton script [ our CGI supports only py ]ß
        {   std::cout << "Invalid file extension for CGI\n"; 
            c->setResponseString(BAD_GATEWAY,"",""); return false;      };
    // ------
    // METHOD
    // ------                              
    std::string method = req.get_method();
    if (method.compare("GET") != 0                                              // only methods get and post are accepted for cgi
        && method.compare("POST") != 0)
        {   std::cout << "Invalid method for CGI\n";
            c->setResponseString(METHOD_NOT_ALLOWED,"","");    return false;              };
    // ------
    // CONTENT LENGTH
    // ------
    if (method == "POST")
    {
        if (header.find("Content-Length") == header.end())
        {   std::cout << "No content length for post method CGI\n"; 
            c->setResponseString(LENGTH_REQUIRED,"","");    return false;  };
        size_t content_length = std::stoi(header["Content-Length"]);
        if (content_length <= 0)
        {   std::cout << "No content length for post method CGI\n";
            c->setResponseString(LENGTH_REQUIRED,"","");    return false;  };
    }
    // ------
    // SCRIPT -> root + action
    // ------
    std::string script = path_to_script + action;
    std::cout << "SCRIPT: " << script << std::endl;
    if (access(script.c_str(), X_OK) < 0)                                        // if executable exists and it's executable
        {   std::cout << "Script " << script << " not executable by CGI\n";
            c->setResponseString(BAD_GATEWAY,"","");    return false;            };
    c->setCGIrequest(true);
    _request.action = action;
    set_CGIrequest(req, req.get_header_map(), path_to_script, upload_store, _server);
    exec_CGI(req, c);      
    return true;
}

/*  DIFFERENCE between POST and GET
[ source: https://www.tutorialspoint.com/perl/perl_cgi.htm ]

The GET method is:
- the defualt method to pass information from browser to web server
- info is written in the URL and is visible to everyone, thus not safe, ie − http://www.test.com/cgi-bin/hello.cgi?key1=value1&key2=value2
- size limtation: only 1024 characters can be in a request string
- info is passed using QUERY_STRING header and  accessible through QUERY_STRING environment variable

The POST method id:
- more reliable
- information is sent as a separate message
- this message comes into the CGI script as standard input

Ex.:
if ($ENV{'REQUEST_METHOD'} eq \"POST\") {
   read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'}); <--- POST 
} else {
   $buffer = $ENV{'QUERY_STRING'}; <--- GET
}
*/

void    Cgi::child_process(Request const& req, Client *c) const
{
    char    *cmd[5];
    std::string pwd = getenv("PWD"); 
    (void)c;
    
    (void)req;
    // if (dup2(_fds[READ], STDIN_FILENO) < 0)                                                  // in the child the output is written to the end of the pipe
    //     {    perror("cgi dup2 in"); exit(EXIT_FAILURE);  }
    if (dup2(_fds[WRITE], STDOUT_FILENO) < 0)
        {    perror("cgi dup2 in"); exit(EXIT_FAILURE);  }
    // we populate cmd[3] for execve                                 
    string2charstar(&cmd[0], get_CGIscript(_request.action).c_str());                           // cmd[0] -> /usr/bin/python                
    string2charstar(&cmd[1], (_request.path_to_script + _request.action).c_str());              // cmd[1] -> cgi-script.py
    cmd[2] = 0;
    close(_fds[WRITE]);
    close(_fds[READ]);
    if (execve(cmd[0], cmd, getEnv()) < 0)
    {    perror("cgi execve"); exit(1);  }
}

void    Cgi::parent_process(int status, Client *c) const
{
    // close(_fds[READ]);
    close(_fds[WRITE]);                                                             // in the parent the output written to the end of the pipe
    waitpid(_pid, &status, 0);                                                      // is re-written to the response to be sent to the server
    
    if (!fdopen(_fds[READ], "r"))
    {   write(2, "BAD FD\n", 7);   c->setResponseString(BAD_GATEWAY, "", "");  return;     }                                                                 // to check if file can be opened, else error
    std::string     _response = file2string(_fds[READ]); 
    std::cerr << "RES: " << _response;    
    if (_response.size())                                                           // if we have an output, execve has succeded                                        
    {   write(2, "PARENT OK\n", 10); c->setResponseString(OK, _response,"");   return;     }
    write(2, "PARENT NOK\n", 11);
    c->setResponseString(BAD_GATEWAY, "", "");                                      // if execve has failed, we send error BAD_GATEWAY
}

void    Cgi::exec_CGI(Request const& req, Client *c)
{
    int status = 0;
    
    if (pipe(_fds) < 0)
    {    perror("cgi pipe"); exit(EXIT_FAILURE);  }
    _pid = fork();
    if (_pid < 0)
    {    perror("cgi fork"); exit(EXIT_FAILURE);  }
    if (_pid == 0)
        child_process(req, c);
    else
        parent_process(status, c);
}

// ************
// SETTERS functions ( + UNSETTERS )
// ************


/*  CGI ENV from http://www.cgi101.com/book/ch3/text.html
Key 	            Value
DOCUMENT_ROOT 	    The root directory of your server
HTTP_COOKIE 	    The visitor's cookie, if one is set
HTTP_HOST 	        The hostname of the page being attempted
HTTP_REFERER 	    The URL of the page that called your program
HTTP_USER_AGENT 	The browser type of the visitor
HTTPS 	            \"on\" if the program is being called through a secure server
QUERY_STRING 	    The query string 
REMOTE_ADDR 	    The IP address of the visitor
REMOTE_HOST 	    The hostname of the visitor (if your server has reverse-name-lookups on; otherwise this is the IP address again)
                    // yeah, totally, we have reverse-name-lookups on
REMOTE_USER 	    The visitor's username (for .htaccess-protected pages)
REQUEST_METHOD 	    GET or POST
REQUEST_URI 	    The interpreted pathname of the requested document or CGI (relative to the document root)
SCRIPT_FILENAME 	The full pathname of the current CGI
SCRIPT_NAME 	    The interpreted pathname of the current CGI (relative to the document root)
SERVER_NAME 	    Your server's fully qualified domain name (e.g. www.cgi101.com)
SERVER_PORT 	    The port number your server is listening on
SERVER_SOFTWARE 	The server software you're using (e.g. Apache 1.3) 
*/


// information are parsed from HTTP request header and body
// https://developer.mozilla.org/en-US/docs/Glossary/Request_header
// NOTE on PATH_INFO/PATH_TRANSLATED vs SCRIPT_NAME/SCRIPT_FILENAME > here we treat them as equivalent
// more on this: https://stackoverflow.com/questions/279966/php-self-vs-path-info-vs-script-name-vs-request-uri
void    Cgi::set_CGIenv(Request const &req, std::map<std::string, std::string> header, ServerInfo* server)
{
    _env["AUTH_TYPE"] = "";
    _env["DOCUMENT_ROOT"] = "~/webserv";
    if ( header.find("Content-Length") != header.end() )                                                                                          
	    _env["CONTENT_LENGTH"] = header["Content-Length"];                                          
    if (header.find("Cookies") != header.end())                                                 
	    _env["HTTP_COOKIE"] = header["Cookies"];
    _env["HTTP_HOST"] = header["Host"];
    _env["HTTP_REFERER"] = header["Referer"];
    _env["HTTP_USER_AGENT "] = header["User-Agent"]; 
    _env["HTTPS"] = "off";
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["PATH_INFO"] = req.get_location();                                                         // the path as requested by the client, eg. www.xxx.com/app
	_env["PATH_TRANSLATED"] = _request.path_to_script + _request.action;                            // the actual path to the script
	_env["QUERY_STRING"] = getFromQueryString(req.get_location());            
	_env["REMOTE_HOST"] = getEnvValue("HTTP_HOST");
	_env["REMOTE_ADDR"] = "127.0.0.1";                                                     
	_env["REMOTE_USER"] = "";
	_env["REMOTE_IDENT"] = "";
	_env["REQUEST_METHOD"] = _request.method;
	_env["REQUEST_URI"] = "/app";                                                           
	_env["SCRIPT_NAME"] = _request.action;
	_env["SCRIPT_FILENAME"] = _request.path_to_script + _request.action;
	_env["SERVER_NAME"] = server->getServerName();                                                  // getEnvValue("HTTP_HOST");
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    std::string port = std::to_string(server->getPort());
	_env["SERVER_PORT"] = port;                                                             
	_env["SERVER_SOFTWARE"] = "webserv/1.9";
    _env["UPLOAD_STORE"] = _request.upload_store;
}

void    Cgi::set_CGIrequest(Request req, std::map<std::string, std::string> header, std::string path_to_script, std::string upload_store, ServerInfo* server)
{
    _request.method = req.get_method();
    if ( header.find("Content-Length") != header.end() )
        _request.content_length = std::stoi(header["Content-Length"]);
    _request.path_to_script = path_to_script;
    _request.upload_store = upload_store;
    set_CGIenv(req, header, server);  
};

void    Cgi::clear_CGIrequest()
{
    _request.action = "";
    _request.method = "";
    _request.content_length = 0;    
    _request.upload_store = "";    
    _request.path_to_script = "";    
};

std::string     Cgi::set_CGIparam(std::string html_content, size_t &pos)
{
    std::string str;
    while ( html_content[++(pos)] != '\0'
    && html_content[(pos)] != '\"')
    str += html_content[(pos)];
    return str;
}

// ************
// GETTERS functions
// ************

std::string     Cgi::getEnvValue(std::string key)              {       return _env[key];       };

// return _env as char**
char**          Cgi::getEnv() const                                          
{
    char**  env_arr;
    size_t  i = -1;
    std::map<std::string, std::string>::const_iterator it = _env.begin();

    env_arr = new char* [ _env.size() + 1 ];
    while ( it != _env.end() )
    {
		std::string	curr = (*it).first + '=' + (*it).second;                // python cgi module splits env on =
		string2charstar(&env_arr[++i], curr.c_str()); it++;
    }
	env_arr[i] = 0;
	return env_arr;
}

bool            Cgi::get_CGIparam(std::string param, std::string html_content, size_t &pos)
{
    std::cout << html_content << std::endl;
    pos = html_content.find(param);
    if (pos == std::string::npos)
        {   std::cout << "Invalid " << param << " for CGI\n"; return false;              };
    pos += param.size() + 1;                                            // the length of the param string + 1 for the =, we start from after that
    return true; 
}

std::string     Cgi::getFromQueryString(std::string uri)    const
{
    size_t pos = uri.find("?") + 1;   
    return uri.substr(pos, uri.size() - pos);
}

CGIrequest&     Cgi::get_CGIrequest()                           {   return _request;                   }           
std::string     Cgi::get_CGIaction()                            {   return get_CGIrequest().action;    }           
std::string     Cgi::get_CGImethod()                            {   return get_CGIrequest().method;    }           
size_t          Cgi::get_CGIcontent_length()                    {   return get_CGIrequest().content_length;    }   
std::string     Cgi::get_CGIscript(std::string action)   const  {   if (action[action.size() - 1] == 'y')  return "/usr/bin/python";   return "/usr/bin/perl";  } 

// ************
// UTILS functions
// ************

void   Cgi::string2charstar(char** charstar, std::string str)   const
{
    *charstar = new char[ str.size() + 1 ];
    strcpy(*charstar, str.c_str());
}

std::string Cgi::file2string(int fd) const
{
    // WORKS ONLY ON MAC
    /*struct stat sb;
    std::string res;

    if (fstat(fd, &sb) < 0)
        perror("Fstat:");
    std::cerr << sb.st_size << std::endl;
    res.resize((int)sb.st_size);
    read(fd, (char*)(res.data()), (int)sb.st_size);
    close(fd);

    return res;*/

    //+++++++++++++
    // char filePath[PATH_MAX];
    // if (fcntl(fd, F_GETPATH, filePath) != -1)
    // {
    //     std::ifstream input_file(filePath);
    //     if (!input_file.is_open()) {
    //         exit(EXIT_FAILURE);
    // }
    // return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
    //++++++++++++++

    std::string res;
    char buf[MAX_SIZE];
	while (int r = read(fd, buf, MAX_SIZE))
	{		buf[r] = '\0';  res += std::string(buf);     }
	close(fd);
	return res;
}
