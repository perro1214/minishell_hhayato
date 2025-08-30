NAME = lexer_parser

CC = cc
CFLAGS = -Wall -Wextra -Werror

SRCDIR = .
SOURCES = lexer.c parser.c command_execution.c env_utils.c expander.c main.c
OBJECTS = $(SOURCES:.c=.o)

LIBFT_DIR = 42_libft
LIBFT = $(LIBFT_DIR)/libft.a

all: $(LIBFT) $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(OBJECTS) $(LIBFT)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LIBFT)

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all

test: $(NAME)
	@echo "==============================================="
	@echo "    Minishell - Lexer/Parser/Executor Test    "
	@echo "==============================================="
	@echo
	@echo "📝 基本コマンドテスト"
	@echo "----------------------------------------"
	@echo 'echo hello world' | ./$(NAME)
	@echo
	@echo "📝 パイプラインテスト"
	@echo "----------------------------------------"
	@echo 'ls -la | grep test > output.txt' | ./$(NAME)
	@echo
	@echo "📝 複合リダイレクションテスト"
	@echo "----------------------------------------"
	@echo 'cat < input.txt | wc -l >> count.txt' | ./$(NAME)
	@echo
	@echo "📝 ヒアドキュメントテスト"
	@echo "----------------------------------------"
	@echo 'cat << EOF' | ./$(NAME)
	@echo
	@echo "📝 クォート処理テスト"
	@echo "----------------------------------------"
	@echo 'echo "hello world" '\''single quoted'\'' | cat' | ./$(NAME)
	@echo
	@echo "📝 オプション処理テスト"
	@echo "----------------------------------------"
	@echo 'echo -n hello' | ./$(NAME)
	@echo 'echo hello -n' | ./$(NAME)
	@echo 'ls -la -h' | ./$(NAME)
	@echo 'grep -i test file.txt' | ./$(NAME)
	@echo 'cat -n file1.txt file2.txt' | ./$(NAME)
	@echo
	@echo "📝 複合オプションテスト"
	@echo "----------------------------------------"
	@echo 'ls -la | grep -v test > output.txt' | ./$(NAME)
	@echo 'echo -n "hello" | cat -n' | ./$(NAME)
	@echo 'find . -name "*.c" -type f' | ./$(NAME)
	@echo
	@echo "📝 引数位置テスト"
	@echo "----------------------------------------"
	@echo 'echo hello world -n' | ./$(NAME)
	@echo 'ls file.txt -la' | ./$(NAME)
	@echo 'grep test -i file.txt' | ./$(NAME)
	@echo
	@echo "==============================================="
	@echo "           テスト完了                          "
	@echo "==============================================="

.PHONY: all clean fclean re test