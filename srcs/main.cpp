/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/12 20:39:03 by mlazzare          #+#    #+#             */
/*   Updated: 2022/09/28 08:19:40 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../inc/Webserv.hpp"

int main(int argc, char **args, char **envp)
{
    Config  config;

    if (argc != 2)
    {
        std::cout << "Please provide ONE argument only\nUsage: ./webserv [configuration file]\n";
        return EXIT_FAILURE;
    }

    if (config.is_valid(args[1], envp))
    {
        try
        {
            Webserv webserv(config.getServers());
            webserv.run_server();
        }
        catch (WebException<std::string>& e)
        {
            std::cout << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        catch (WebException<int>& e)
        {
            std::cout << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        // catch (...)
        // {
		//     std::cout << "Unrecognized error\nClosing server...\n";
		//     return EXIT_FAILURE;
	    // }
    }

    return EXIT_SUCCESS;
}
