
NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98

SRCS	=	srcs/main.cpp											

INC		=	inc/

all: $(NAME)

$(NAME): $(SRCS) $(INC)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

clean: fclean

fclean:
	@rm -f $(NAME)

re: fclean all

.PHONY: re clean fclean all