/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 13:10:34 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/19 14:44:31 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Cgi.hpp"

Cgi::Cgi()                  {}
Cgi::~Cgi()                 {}

// checks action, method, content-length in html message 
// returns true if it's good for cgi and set CGI request if so
bool        isCGI_request(Request const &req)
{
    (void)req;
    // std::string pwd = getenv("PWD");
    // std::string root = pwd + "/cgi-bin/";                     // hardcoded here; this should be retrieved from ServerInfo > page > root
    // size_t pos = 0;
    // // ------
    // // ACTION
    // // ------
    // if (get_CGIparam("action", html_content, pos) == false)                 // action="........", we want to start from the first \" after the =
    //     return false;
    // std::string action = set_CGIparam(html_content, pos);
    // // std::cout << action << std::endl;
    // size_t extension = action.size() - 3;
    // if (action.compare(extension, action.size(), ".py")                     // check if it's a pyhton or perl script [ our CGI supports only py and perl ]
    //     && action.compare(extension, action.size(), ".pl"))
    //     {   std::cout << "Invalid file extension for CGI\n"; return false;      };
    // // ------
    // // METHOD
    // // ------
    // if (get_CGIparam("method", html_content, pos) == false)
    //     return false;                                
    // std::string method = set_CGIparam(html_content, pos);
    // if (method.compare("get") != 0                                              // only methods get and post are accepted for cgi
    //     && method.compare("post") != 0)
    //     {   std::cout << "Invalid method for CGI\n"; return false;              };
    // // ------
    // // CONTENT LENGTH
    // // ------
    // //if (method == "post" && get_CGIparam("Content-Length", html_content, pos) == false) // !!! Content-Length is in header, not in the body, cannot be parsed like this
    //     //return false;                                                                   // hard-coded here for the moment to test post method                               
    // size_t content_length = std::stoi("100");
    // if (method == "post" && !content_length)
    //     {   std::cout << "No content length for post method CGI\n"; return false;  };
    // // ------
    // // SCRIPT -> root + action
    // // ------
    // root += action;
    // if (access(root.c_str(), X_OK) < 0)                                         // if executable exists and it's executable
    //     {   std::cout << "Script not executable by CGI\n"; return false;  };
    // _request.path_to_script = root;
    // set_CGIrequest(action, method, 0);
    // set_CGIenv(html_content);          
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
- information is sends it as a separate message
- this message comes into the CGI script in the form of the standard input

Ex.:
if ($ENV{'REQUEST_METHOD'} eq \"POST\") {
   read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'}); <--- POST 
} else {
   $buffer = $ENV{'QUERY_STRING'}; <--- GET
}
*/

void    Cgi::child_process(CGIrequest& req)
{
    char    *cmd[3]; 
    
    // if (req.method == "post")                                                       // if post method, we write content to stdin
    // {
    //     size_t pos = html_content.find("\r\n\r\n") + 4;
    //     std::string content = html_content.substr(pos, req.content_length - pos);
    //     write(_fds[READ], content.c_str(), content_length);
    // }
    if (dup2(_fds[READ], STDIN_FILENO) < 0)                                         // in the child the output is written to the end of the pipe
        {    perror("cgi dup2 in"); exit(EXIT_FAILURE);  }
    // if (req.socket_fd)                                                           // we write the output to socket fd to send to the server
    //     _fds[WRITE] = req.socket_fd;
    // if (dup2(_fds[WRITE], STDOUT_FILENO) < 0)
    //     {    perror("cgi dup2 in"); exit(EXIT_FAILURE);  }
    // we populate cmd[3] for execve                                   
    string2charstar(&cmd[0], get_CGIscript(req.action).c_str());                    // cmd[0] -> /usr/bin/python                
    string2charstar(&cmd[1], req.path_to_script.c_str());                           // cmd[1] -> cgi-script.py
    cmd[2] = 0;
    close(_fds[WRITE]);
    if (execve(cmd[0], cmd, getEnv()) < 0)
    {    perror("cgi execve"); exit(EXIT_FAILURE);   }
}

void    Cgi::parent_process(int status)
{    
    close(_fds[READ]);
    close(_fds[WRITE]);                                                              // in the parent the output written to the end of the pipe
    waitpid(_pid, &status, 0);                                                       // is re-written to the socket_fd to be sent to the server
}

void    Cgi::exec_CGI(CGIrequest& req)
{
    int status = 0;
    
    if (pipe(_fds) < 0)
    {    perror("cgi pipe"); exit(EXIT_FAILURE);  }
    _pid = fork();
    if (_pid < 0)
    {    perror("cgi fork"); exit(EXIT_FAILURE);  }
    if (_pid == 0)
        child_process(req);
    else
        parent_process(status);
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
void    Cgi::set_CGIenv(std::string html_content)
{
    size_t pos = 0;
    std::string pwd = getenv("PWD");
   
    _env["AUTH_TYPE"] = "";
    _env["DOCUMENT_ROOT"] = "~/webserv";                                                     // add a function get_pwd?                                  
	_env["CONTENT_LENGTH"] = set_CGIparam(html_content, pos);                                // because we need it as string, _request.content_length is an int XXXX
    // if (get_CGIparam("Cookie", html_header, pos))                                         // need a request with html_header attribute
	//     _env["HTTP_COOKIE"] = set_CGIparam(html_header, pos);
    // if (get_CGIparam("Host", html_header, pos))
	//     _env["HTTP_HOST"] = set_CGIparam(html_header, pos);
    // if (get_CGIparam("Referer", html_header, pos))
	//     _env["HTTP_REFERER"] = set_CGIparam(html_header, pos);
    // if (get_CGIparam("User-Agent", html_header, pos))
	//     _env["HTTP_USER_AGENT "] = set_CGIparam(html_header, pos);
    _env["HTTPS"] = "off";
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["PATH_INFO"] = "/app";                                                             // the path as requested by the client, eg. www.xxx.com/app
	_env["PATH_TRANSLATED"] = pwd + "/cgi-bin/" + _request.action;            // the actual path to the script
	_env["QUERY_STRING"] = getFromQueryString("blabla.com/en?name=Undi&age=11");            // hard-coded here, to be fetched from request
	// _env["REMOTE_HOST"] = getEnvValue("HTTP_HOST");
	_env["REMOTE_ADDR"] = "127.0.0.1";                                                      // hard-coded here, to be fetched from request
	_env["REMOTE_USER"] = "";
	_env["REMOTE_IDENT"] = "";
	_env["REQUEST_METHOD"] = _request.method;
	_env["REQUEST_URI"] = "/app";                                                           // hard-coded here, to be fetched from request
	_env["SCRIPT_NAME"] = _request.action;
	_env["SCRIPT_FILENAME"] = pwd + "/cgi-bin/" + _request.action;
	_env["SERVER_NAME"] = "codedinbelgium.be";                                              // getEnvValue("HTTP_HOST");
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_PORT"] = "80";                                                             // hard-coded here, to be fetched from request
	_env["SERVER_SOFTWARE"] = "webserv/1.9";
	                                                   // hard-coded here, to be fetched from config

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

std::string     Cgi::getEnvValue(std::string key)       {       return _env[key];       };

// return _env as char**
char**          Cgi::getEnv()                                           
{
    char**  env_arr;
    size_t  i = -1;
    std::map<std::string, std::string>::iterator it = _env.begin();

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

std::string     Cgi::getFromQueryString(std::string uri)
{
    size_t pos = uri.find("?") + 1;   
    return uri.substr(pos, uri.size() - pos);
}

CGIrequest&     Cgi::get_CGIrequest()                    {   return _request;    }
std::string     Cgi::get_CGIaction()                     {   return get_CGIrequest().action;    }
std::string     Cgi::get_CGImethod()                     {   return get_CGIrequest().method;    }
size_t          Cgi::get_CGIcontent_length()             {   return get_CGIrequest().content_length;    }
std::string     Cgi::get_CGIscript(std::string action)   {   if (action[action.size() - 1] == 'y')  return "/usr/bin/python";   return "/usr/bin/perl";  }

// ************
// HTTP HEADERS functions
// ************

// information are sent to the HTTP response header
// https://developer.mozilla.org/en-US/docs/Glossary/Response_header
void    Cgi::http_header()
{
    std::cout << "200 OK\n";
    std::cout << "Content-Type: text/html; charset=utf-8;";
    std::cout << "Set-Cookie: Cookies are set\r\n\r\n";
}

void    Cgi::redirect_http_header(std::string loc)
{
    std::cout << "Status:308 Permanent redirect\nLocation: ";
    std::cout << loc;
    std::cout << "\r\n\r\n";
}

// ************
// UTILS functions
// ************

void   Cgi::string2charstar(char** charstar, std::string str)
{
    *charstar = new char[ str.size() + 1 ];
    strcpy(*charstar, str.c_str());
}