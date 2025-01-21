# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vagarcia <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/20 16:37:43 by vagarcia          #+#    #+#              #
#    Updated: 2024/09/25 12:06:17 by vagarcia         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#Variables
NAME = libftprintf.a

SRC_DIR = src/

OBJ_DIR = obj/

CC = cc

AR = ar rcs

NOMOREMAKE = .make_made_the_making

CFLAGS = -c -Wall -Wextra -Werror

#colors

DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m

#sources

PRINTFILES = ft_printf.c ft_printhex.c ft_printptr.c ft_printunsigned.c \
			 ft_printint.c \

PRINTFO = $(PRINTFILES:.c=.o)

### 

$(NAME): $(PRINTFO)
	$(AR) $(NAME) $(PRINTFO)
	@echo "$(GREEN) ft_printf compiled! $(DEF_COLOR)"

.PHONY: all clean fclean re

all:  $(NAME)

clean :
	rm -f $(PRINTFO)
	@echo "$(MAGENTA) Erasing object files for ft_printf... $(DEF_COLOR)"

fclean : clean
	rm -f $(NAME)
	@echo "$(MAGENTA) Everything ready for re-make $(DEF_COLOR)"

re: fclean all
	@echo "$(CYAN) Cleaned and redid everything for ft_printf..."
