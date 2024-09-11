CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
NAME = ircserv

SRC = main.cpp \
	  src/server.cpp \
	  src/client.cpp \
	  src/utils.cpp \
	  src/Command.cpp \
	  src/Channel.cpp

RM = rm -rf
OBJ = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJ)
		  $(CC) $(OBJ) -o $(NAME) -g
		  @printf "\e[92;5;118m\n>>> Executable ready\n\e[0m"

%.o : %.cpp
	$(CC) $(FLAGS) -c $< -o $@ -g

clean :
	@printf "\033[00;31m>>> Object files removed.\n\033[00m"
	$(RM) $(OBJ)

fclean :	clean
		@printf "\033[00;31m>>> Executable removed.\n\033[00m"
		$(RM) $(NAME)

re : fclean all