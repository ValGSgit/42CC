NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -MD -MP
RM = rm -rf

# Directories
SRCDIR = src
OBJDIR = obj
INCDIR = includes

# Source files (mandatory part - without session/cookie managers)
SRCFILES = main.cpp \
		   config/ConfigParser.cpp \
		   cgi/CgiHandler.cpp \
		   http/HttpRequest.cpp \
		   http/HttpResponse.cpp \
		   http/HttpTemplates.cpp \
		   http/HttpHandler.cpp \
		   server/ServerManager.cpp \
		   utils/Utils.cpp

# Bonus files (session and cookie management)
BONUS_FILES = utils/SessionManager.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRCFILES))
BONUS_SRCS = $(addprefix $(SRCDIR)/, $(BONUS_FILES))
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
BONUS_OBJS = $(BONUS_SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Include flags
INCLUDES = -I$(INCDIR)

# Bonus flag file
BONUS_FLAG = .bonus

all: $(NAME)

bonus: CXXFLAGS += -D BONUS
bonus: fclean $(BONUS_FLAG)

$(BONUS_FLAG): $(OBJS) $(BONUS_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(BONUS_OBJS) -o $(NAME)
	@touch $(BONUS_FLAG)
	@echo "Bonus features compiled (Session & Cookie Management)"

# Valgrind rule with suppression file
vg: $(NAME)
	@echo "Starting WebServ with Valgrind..."
	@echo "Log file: valgrind_$(shell date +%Y%m%d_%H%M%S).log"
	@echo "Press Ctrl+C to stop the server"
	@valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         --track-fds=yes \
	         --trace-children=yes \
	         --suppressions=valgrind.supp \
	         ./$(NAME) webserv.conf
#--log-file=valgrind_$(shell date +%Y%m%d_%H%M%S).log \

%.d:

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)
	$(RM) $(BONUS_FLAG)

re: fclean all

test: $(NAME)
	./$(NAME) config/default.conf

test_config: test_config_parser
	./test_config_parser config/default.conf

test_config_parser: tests/scripts/test_config_parser.cpp $(filter-out $(OBJDIR)/main.o, $(OBJS))
	$(CXX) $(CXXFLAGS) $(INCLUDES) tests/scripts/test_config_parser.cpp $(filter-out $(OBJDIR)/main.o, $(OBJS)) -o test_config_parser

$(OBJDIR)/testmain.o: $(SRCDIR)/testmain.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Testing targets
run_tests: $(NAME)
	@echo "Starting comprehensive test suite..."
	@./run_tests.sh

test_full: run_tests

test_edge_cases: $(NAME)
	@echo "========================================"
	@echo "Running comprehensive edge case tests..."
	@echo "Make sure the server is running first!"
	@echo "========================================"
	@./tests/comprehensive_edge_cases.sh

test_security: $(NAME)
	@echo "Starting server and running security tests..."
	@./webserv webserv.conf > server_test.log 2>&1 & \
	SERVER_PID=$$!; \
	sleep 3; \
	python3 tests/security/run_all_security_tests.py; \
	TEST_EXIT=$$?; \
	kill $$SERVER_PID 2>/dev/null || true; \
	exit $$TEST_EXIT

test_sessions: $(NAME)
	@echo "Starting server and running session/cookie tests..."
	@./webserv webserv.conf > server_test.log 2>&1 & \
	SERVER_PID=$$!; \
	sleep 3; \
	bash tests/session_cookie/run_all_tests.sh; \
	TEST_EXIT=$$?; \
	kill $$SERVER_PID 2>/dev/null || true; \
	exit $$TEST_EXIT


#
.PHONY: all clean fclean re test test_config test_config_parser bonus run_tests test_full test_edge_cases test_security test_sessions security_audit vg

