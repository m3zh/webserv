#  include "../inc/Config.hpp"

// *****************************
// If Lexer returns a valid config,
// the Config class sets the value to pass to the class Webserv
// *****************************

Config::Config()      {};
Config::~Config()     {};

int     Config::read(char   *config, char **envp)
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

void    Config::setServers(ServerInfo &server) {    _servers.push_back(server); std::cout << _servers.size(); };

void    Config::setServerParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it)
{
    while (it != parser.tokens.end() && it->getType() == "Key")
    {
        Token key_tmp = *it;  // save la clef avant d'itérer sur la/les valeur(s) 
        it++;
        while (it != parser.tokens.end() && it->getType() != "Key" && it->getType() != "Namespace")
        {
            std::cout << key_tmp.getContent() + '\n';
            if (key_tmp.getContent() == "listen")
                server.setPort(stoi(it->getContent()));
            else if (key_tmp.getContent() == "server_name")
                server.setServerName(it->getContent());
            else if (key_tmp.getContent() == "client_max_body_size")
                server.setClientMaxBodySize(stoi(it->getContent()));
            else if (key_tmp.getContent() == "root")
                server.setServerRoot(it->getContent());
            else if (key_tmp.getContent() == "index")
                server.setServerIndex(it->getContent());
            it++;
        }
    }
}

void    Config::setServerPageParams(Lexer &parser, ServerInfo &server, std::vector<Token>::iterator &it)
{
    while (it != parser.tokens.end() && it->getContent() == "location")
    {
        page    p;
        
        it++;
        p.location_path = it->getContent();
        it++;
        while (it != parser.tokens.end() && it->getType() != "Namespace")
        {
            Token key_tmp = *it;  // save la clef avant d'itérer sur la/les valeur(s) 
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
        if ((*pit).location_path.compare("/") == 0)                         // check if there is ONE root location /
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