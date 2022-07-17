# include "../inc/Config.hpp"

int main(int argc, char **args)
{
    Config config;

    if (argc != 2)
    {
        std::cout << "Please provide ONE argument only\nUsage: ./webserv [configuration file]\n";
        return EXIT_FAILURE;
    }
    if (config.read(args[1]))
        std::cout << "Webserv class starts here\n";
    /* ICI to DEBUG Config: print tokens params */
    return EXIT_SUCCESS;
}
