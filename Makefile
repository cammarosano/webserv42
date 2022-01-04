SRC = ListenEndPoint.cpp  Vserver.cpp  main.cpp
HEADER = HttpRequest.hpp  ListenEndPoint.hpp  OutboundResponse.hpp  \
			Route.hpp  utils.hpp  Vserver.hpp
OBJ = $(SRC:.cpp=.o)
CC = clang++
CFLAGS = -Wall -Wextra -Werror -std=c++98
NAME = webserv

all:	$(NAME)

$(NAME):	$(OBJ)
			$(CC) $(CFLAGS) $^ -o $@

%.o:		%.cpp $(HEADER)
			$(CC) $(CFLAGS) -c $< -o $@

clean:
			rm -f $(OBJ)

fclean:		clean
			rm -f $(NAME)

re:			clean all

.PHONY:		all clean fclean re
