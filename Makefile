NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -g #-std=c++98 -g

SRCS	=	srcs/main.cpp		\
			srcs/Token.cpp		\
			srcs/Lexer.cpp		\
			srcs/Config.cpp									

INC		=	inc/Token.hpp		\
			inc/Lexer.hpp		\
			inc/Config.hpp

all: $(NAME)

$(NAME): $(SRCS) $(INC)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

clean: fclean

fclean:
	@rm -f $(NAME)

re: fclean all

.PHONY: re clean fclean all
