NAME = lexer_parser

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCDIR = .
SOURCES = lexer.c parser.c main.c
OBJECTS = $(SOURCES:.c=.o)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS)

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME)

re: fclean all

test: $(NAME)
	@echo "Testing lexer and parser..."
	@echo 'echo hello world' | ./$(NAME)
	@echo 'ls -la | grep test > output.txt' | ./$(NAME)
	@echo 'cat < input.txt | wc -l >> count.txt' | ./$(NAME)
	@echo 'cat << EOF' | ./$(NAME)

.PHONY: all clean fclean re test