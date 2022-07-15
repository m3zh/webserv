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

    /* parser: print tokens params
    for (size_t i = 0; i < config.tokens.size(); i++)
    {
        std::cout << "ici\n";
         for (size_t j = 0; j < config.tokens[i].size(); j++)
        {
            std::cout   << "Line " << i << ", Word " << j << "; type= "
                        << config.tokens[i][j].getType() << "; pos= "
                        << config.tokens[i][j].getPos() << "; content= "
                        << config.tokens[i][j].getContent() << std::endl;
        }
    }
    for (auto it = begin (config.tokens); it != end (config.tokens); ++it) {
        it->doSomething ();
    }
    */
    return EXIT_SUCCESS;
}
