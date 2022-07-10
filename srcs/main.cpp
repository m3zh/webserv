/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 18:20:35 by mlazzare          #+#    #+#             */
/*   Updated: 2022/07/10 23:20:06 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>
# include <cstring>
# include <stdlib.h>
# include "../inc/Parser.hpp"

int main(int argc, char **args)
{
    if (argc != 2)
    {
        std::cout << "Please provide ONE argument only\nUsage: ./webserv [configuration file]\n";
        return EXIT_FAILURE;
    }
    Parser config;
    if (config.read(args[1]))
        config.getPort();
        //webserv(config);
    return EXIT_SUCCESS;
}
