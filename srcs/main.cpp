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
    config.read(args[1]);

    /* ICI
        parser: print tokens params
    */
    for (auto it = begin (config.tokens); it != end (config.tokens); ++it) {
        std::cout  << "type= "
                    << it->getType() << "; pos= "
                    << it->getPos() << "; content= "
                    << it->getContent() << std::endl;
    }
    return EXIT_SUCCESS;
}
