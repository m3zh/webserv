/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlazzare <mlazzare@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/10 18:20:35 by mlazzare          #+#    #+#             */
/*   Updated: 2022/07/10 18:24:59 by mlazzare         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>
# include <cstring>
# include <stdlib.h>

int main(int argc, std::string *args)
{
    if (argc != 2)
    {
        std::cout << "Please provide ONE argument only\nUsage: ./executable config.file\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
