/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/05 22:35:05 by mlazzare          #+#    #+#             */
/*   Updated: 2022/10/16 21:13:13 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#  include "../inc/Config.hpp"

Config::Config()      {};
Config::~Config()     {};

int     Config::is_valid(char   *config, char **envp)
{
    Lexer parser;

    if (parser.read(config, envp))
    {        
        std::vector<Token>::iterator    it = parser.tokens.begin();
        // si la config est valide, on parcourt les tokens
        while (it != parser.tokens.end() && it->getContent() == "server")
        {
            // si le token est un server, on l'instancie
            ServerInfo  server;

            it++;
            setServerParams(parser, server, it);
            setServerPageParams(parser, server, it);
            setServers(server);
        }
        // debug_me(parser);
        if (valid_config(getServers()))
            return 1;
    }
    std::cout << "Error in config\n"; 
    // debug_me(parser);
    return 0;
}

// ************
// SETTER functions
// ************

void    Config::setServers(ServerInfo &server)          {    _servers.insert(_servers.begin(), server); };

int    Config::setServerParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it)
{
    while (it != parser.tokens.end() && it->getType() == "Key")
    {
        Token key_tmp = *it;                                    // save la clef avant d'itérer sur la/les valeur(s) 
        it++;
        while (it != parser.tokens.end() && it->getType() != "Key" && it->getType() != "Namespace")
        {
            // ports valides entre 0 et 65535 https://www.webopedia.com/reference/well-known-tcp-port-numbers/
            // pour le port : check si it nest pas vide, si cest un nombre qui ne depasse pas (int ?) et quil est unique
            if (key_tmp.getContent() == "listen")
            {
                int port;
                if (server.getPort() != -1)
                {   std::cout << "Too many ports\n"; return 0;  }
                try { 
                    port = stoi(it->getContent());
                    server.setPort(port);
                }
                catch (...) {
                    std::cout << "Something wrong with stoi in setServerParams...\n";
                    return 0;
                }
                if (port < 0 || port > 65535)
                {   std::cout << "Wrong port number\n"; return 0;  }
                
            }
            else if (key_tmp.getContent() == "server_name")
            {
                if (server.getServerName() != "")
                {   std::cout << "Too many server names\n"; return 0;  }
                server.setServerName(it->getContent()); 
            }
            else if (key_tmp.getContent() == "client_max_body_size")
            {
                if (server.getClientMaxBodySize() != 1048576)
                {   std::cout << "Too many body sizes\n"; return 0;  }
                server.setClientMaxBodySize(stoi(it->getContent())); 
            }
            else if (key_tmp.getContent() == "root")
            {
                if (server.getServerRoot() != "")
                {   std::cout << "Too many server indexes\n"; return 0;  }
                server.setServerRoot(it->getContent());
            }
            else if (key_tmp.getContent() == "index")
            {
                if (server.getServerIndex() != "")
                {   std::cout << "Too many server indexes\n"; return 0;  }
                server.setServerIndex(it->getContent());
            }
            it++;
        }
    }
    return 1;
}

void    Config::setServerPageParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it)
{
    while (it != parser.tokens.end() && it->getContent() == "location")
    {
        page  p;
        
        it++;
        p.location_path = it->getContent();
        it++;
        while (it != parser.tokens.end() && it->getType() != "Namespace")
        {
            Token key_tmp = *it;                        // save la clef avant d'itérer sur la/les valeur(s) 
            it++;
            while (it != parser.tokens.end() && it->getType() != "Namespace" && it->getType() != "Key")
            {
                if (key_tmp.getContent() == "root")
                    p.root = it->getContent();
                else if (key_tmp.getContent() == "upload_store")
                    p.upload_store = it->getContent();
                else if (key_tmp.getContent() == "redirect")
                    p.redirect = it->getContent();
                 else if (key_tmp.getContent() == "autoindex")
                    p.autoindex = it->getContent();
                else if (key_tmp.getContent() == "allowed_methods")
                {
                    while (it != parser.tokens.end() && it->getType() == "Method")
                    {
                        p.methods.push_back(it->getContent());
                        it++;
                    }
                    break ;
                }
                it++;
            }
        }
        server.setPages(p);
    }
}

// ************
// GETTER functions
// ************

std::vector<ServerInfo>&     Config::getServers()        {   return _servers;    };

// ************
// VALIDATE functions
// ************

bool                    Config::valid_config(std::vector<ServerInfo>& s)
{   
    std::vector<ServerInfo>::iterator it = s.begin();

    while ( it != s.end() - 1)
    {
        int root = 0;
        if ((*it).getPort() < 0) return false;                                  // check if port has been set
        std::vector<page>::iterator pit = (*it).getPages().begin();                 
        while ( pit != (*it).getPages().end() )
        {    if ((*pit).location_path.compare("/") == 0)                         // check if there is ONE root location /
                root++;
            pit++;
        }
        std::vector<ServerInfo>::iterator it2 = it + 1;
        while ( it2 != s.end()) 
        {
            if ((*it).getServerName().compare((*it2).getServerName()) == 0      // check that servers do not run on same port and server name
            && (*it).getPort() == (*it2).getPort())
                return false;
            it2++;
        }
        if (root != 1)
            return false;
        it++;
    }
    int root = 0;
    std::vector<page>::iterator pit = (*it).getPages().begin();                 
    while ( pit != (*it).getPages().end() )
    {           
        if ((*pit).location_path.compare("/") == 0)                             // check if there is ONE root location /
            root++;
        pit++;
    }
    if (root != 1)
        return false;
    return true;
};

// ************
// DEBUGGING functions
// ************

void    Config::debug_me(Lexer &parser)
{
    std::cout << "############### PARSER class ###############\n";
    std::vector<Token>::iterator it = parser.tokens.begin();
    while (it != parser.tokens.end()) {
    std::cout  << "type= "  << it->getType() << "; pos= ";
    std::cout  << it->getPos() << "; content= ";
    std::cout  << it->getContent();
    if (it->getType() == "Key")
        std::cout << "; aw= " << it->getAllowedWords();
    std::cout << std::endl;
        it++;
    }

    std::cout << "############### SERVERINFO class ###############\n";
    std::vector<ServerInfo>::iterator sit = getServers().begin();

    while (sit != getServers().end())
    {
        
        std::cout << "server_name: " << (*sit).getServerName() << ",\n"
                << "  - port: " << (*sit).getPort() << ",\n"
                << "  - client_max:  " << (*sit).getClientMaxBodySize() << ",\n"
                << "  - root:  " << (*sit).getServerRoot() << ",\n"
                << "  - index:  " << (*sit).getServerIndex() << ",\n";

        std::cout << "size pages : " << (*sit).getPages().size() << "\n";
        std::vector<page>::iterator it2 = (*sit).getPages().begin();
        while (it2 != (*sit).getPages().end())
        {
            std::cout << "  - location " << (*it2).location_path << ",\n"
                        << "        - autoindex " << (*it2).autoindex << ",\n"
                        << "        - root " << (*it2).root << ",\n";
                // if (it2->methods[0])
                // {

                //     std::cout   << "        - " << it2->methods[0] << ",\n";
                // }
            it2++;
        }
        it++;
    }
}