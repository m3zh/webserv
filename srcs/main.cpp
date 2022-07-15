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

    /* ICI
        parser: print tokens params
    */
    for (size_t i = 0; i < config.curr_line_tokens.size(); i++)
    {
        std::cout  <<  "Word " << i << "; type= "
                    << config.curr_line_tokens[i].getType() << "; pos= "
                    << config.curr_line_tokens[i].getPos() << "; content= "
                    << config.curr_line_tokens[i].getContent() << std::endl;
        std::cout << "ici\n";
    }

    return EXIT_SUCCESS;
}
