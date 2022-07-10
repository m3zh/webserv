#  include "../inc/Parser.hpp"

Parser::Parser (): _port(0) {};
Parser::~Parser()  {};

int     Parser::read(char   *config)
{
    std::fstream file(config, std::fstream::in);
    if  (file.good())
    {
        std::string  line;
        while(std::getline( file, line ) )
        {
            while (line.length() == 0)
                getline(file, line);
            line = trim(line);
            if ( line.compare(0, 6, "listen") == 0 )
                setPort(line);
        } 
        file.close();     
    }
    else
    {
        std::cout << "Error opening file" << std::endl;
        return 0;
    }
    return 1;
}

std::string     Parser::getIP() const                       {   return _ip;                 };
int             Parser::getPort() const                     {   return _port;               };
std::string     Parser::getProtocol() const                 {   return _protocol;           };

void            Parser::setPort(std::string const s)
{
    
    if (_port != 0)
        return ;
    std::stringstream tmp;
    std::string nums = "0123456789";
    size_t start = s.find_first_of(nums);
    size_t end = s.find_last_of(nums) + 1;
    _port = stoi(s.substr(start, end));
};


std::string     Parser::trim(std::string s)
{
    std::string spaces = " \n\r\t\f\v";
    size_t start = s.find_first_not_of(spaces);
    size_t end = s.find_last_not_of(spaces);
    return s.substr(start, end);
}