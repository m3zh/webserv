# include "../inc/Webserv.hpp"

int main(int argc, char **args, char **envp)
{
    Config  config;

    if (argc != 2)
    {
        std::cout << "Please provide ONE argument only\nUsage: ./webserv [configuration file]\n";
        return EXIT_FAILURE;
    }
    if (config.read(args[1], envp))
    {
        std::cout << "Webserv class starts here\n";
        Webserv webserv(config.getServers());
    }
    return EXIT_SUCCESS;
}
