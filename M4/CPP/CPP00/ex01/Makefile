NAME = Phonebook

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = book_of_phones.cpp \
		get_and_set.cpp \
		main.cpp

OBJS = $(SRCS:.cpp=.o)
HEADERS = phonebook.hpp contact.hpp

# Colors
YELLOW = \033[0;33m
BLUE = \033[0;34m
GREEN = \033[1;32m
ORANGE = \033[0;33m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(BLUE)Linking $(NAME)...$(RESET)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled successfully!$(RESET)"

%.o: %.cpp $(HEADERS)
	@echo "$(YELLOW)Compiling $<...$(RESET)"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "$(ORANGE)Removing object files...$(RESET)"
	@rm -f $(OBJS)

fclean: clean
	@echo "$(ORANGE)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re