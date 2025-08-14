#ifndef LEXER_PARSER_H
# define LEXER_PARSER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdbool.h>

typedef enum e_token_type {
	EXPANDABLE,         // 通常の文字列
	EXPANDABLE_QUOTED,  // ダブルクォート内文字列
	NON_EXPANDABLE,     // シングルクォート内文字列
	PIPE,               // |
	REDIRECT_IN,        // <
	REDIRECT_OUT,       // >
	REDIRECT_APPEND,    // >>
	REDIRECT_HEREDOC,   // <<
	EOF_TOKEN           // 終端
} t_token_type;

typedef struct s_token
{
	t_token_type		type;
	char				*value;
	struct s_token		*next;
}	t_token;

typedef struct s_ast
{
	t_token_type		type;
	char				*value;
	struct s_ast		*left;
	struct s_ast		*right;
}	t_ast;

typedef struct s_parser
{
	t_token				*tokens;
	t_token				*current;
}	t_parser;

// Lexer functions
t_token		*tokenize(const char *input);
t_token		*create_token(t_token_type type, const char *value);
void		add_token(t_token **head, t_token *new_token);
void		free_tokens(t_token *head);
void		print_tokens(t_token *head);

// Parser functions
t_ast		*parse(t_token *tokens);
t_ast		*create_ast_node(t_token_type type, const char *value);
void		free_ast(t_ast *node);
void		print_ast(t_ast *node, int level);

// Helper functions
bool		is_redirect_token(t_token_type type);
bool		is_quote(char c);
bool		is_whitespace(char c);
bool		is_special_char(char c);
int			skip_whitespace(const char *str, int pos);

#endif