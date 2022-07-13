# include <iostream>
# include <cstring>
# include <stdlib.h>

# include "../inc/Lexer.hpp"

int main(int argc, char **args)
{
    Lexer config;
    
    if (argc != 2)
    {
        std::cout << "Please provide ONE argument only\nUsage: ./webserv [configuration file]\n";
        return EXIT_FAILURE;
    }
    if (config.read(args[1]))
        //webserv(config);
    return EXIT_SUCCESS;
}
