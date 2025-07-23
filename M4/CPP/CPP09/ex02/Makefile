# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::   #
#    Makefile                                           :+:      :+:    :+:   #
#                                                     +:+ +:+         +:+     #
#    By: your_login <your_email@student.42.fr>      +#+  +:+       +#+        #
#                                                 +#+#+#+#+#+   +#+           #
#    Created: 2025/07/10 00:00:00 by your_login       #+#    #+#             #
#    Updated: 2025/07/10 00:00:00 by your_login      ###   ########.fr       #
#                                                                              #
# **************************************************************************** #

NAME = PmergeMe

# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

# Source files
SRCDIR = .
SRCS = main.cpp PmergeMe.cpp
OBJS = $(SRCS:.cpp=.o)

# Header dependencies
HEADERS = PmergeMe.hpp

# Default target
all: $(NAME)

# Build target
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# Object file compilation with dependency tracking
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean object files
clean:
	rm -f $(OBJS)

# Clean everything
fclean: clean
	rm -f $(NAME)

# Rebuild everything
re: fclean all

# Test targets for development
test: $(NAME)
	./$(NAME) 3 5 9 7 4

test_error: $(NAME)
	./$(NAME) "-1" "2"

test_large: $(NAME)
	./$(NAME) `seq 1 100 | shuf | head -20 | tr '\n' ' '`

# Phony targets
.PHONY: all clean fclean re test test_error test_large
