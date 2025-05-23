CC		= cc
CFLAGS	= -Wall -Werror -Wextra
SRC_PATH = src/

NAMEC	= client
NAMES	= server
NAMECB	= client_bonus
NAMESB	= server_bonus

SERV = server.c
CLNT = client.c
SERVB = server_bonus.c
CLNTB = client_bonus.c

SRCS	= $(addprefix $(SRC_PATH), $(SERV))
SRCC	= $(addprefix $(SRC_PATH), $(CLNT))
SRCSB	= $(addprefix $(SRC_PATH), $(SERVB))
SRCCB	= $(addprefix $(SRC_PATH), $(CLNTB))
OBJS	= $(addprefix $(OBJ_PATH), $(OBJ))
INCS	= -I ./includes/

all: $(NAMEC) $(NAMES)

bonus: $(NAMECB) $(NAMESB)

$(OBJ_PATH):$(OBJ_PATH) 
	mkdir $(OBJ_PATH)

$(NAMEC): $(SRCC)
	$(CC) $(CFLAGS) $(SRCC) -o $(NAMEC)

$(NAMES): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(NAMES)

$(NAMECB): $(SRCCB)
	$(CC) $(CFLAGS) $(SRCCB) -o $(NAMECB)

$(NAMESB): $(SRCSB)
	$(CC) $(CFLAGS) $(SRCSB) -o $(NAMESB)

clean:
	rm -rf $(OBJ_PATH)

fclean: clean
	rm -f $(NAMES) \
    rm -f $(NAMEC) \
	rm -f $(NAMESB) \
    rm -f $(NAMECB) 

re: fclean all

.PHONY: all clean fclean re
