NAME = PmergeMe
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

ifdef COUNT_COMPARISONS
	CXXFLAGS += -DCOUNT_COMPARISONS
endif

SOURCES = main.cpp PmergeMe.cpp PmergeMeVector.cpp PmergeMeDeque.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


count: clean
	@echo "Building with comparison counting enabled..."
	$(MAKE) COUNT_COMPARISONS=1
	@echo "Build complete with comparison counting enabled"

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re count 
