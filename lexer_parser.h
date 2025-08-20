#ifndef LEXER_PARSER_H
# define LEXER_PARSER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdbool.h>
# include <sys/types.h>

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





// Command execution structures
typedef enum e_redirect_type
{
	REDIR_INPUT,        // <
	REDIR_OUTPUT,       // >
	REDIR_APPEND,       // >>
	REDIR_HEREDOC       // <<
}	t_redirect_type;

typedef struct s_cmd_redirection
{
	t_redirect_type				type;
	char						*file_path;
	int							fd;
	struct s_cmd_redirection	*next;
}	t_cmd_redirection;

typedef struct s_command_invocation
{
	t_cmd_redirection			*output_redirections;
	struct s_command_invocation	*piped_command;
	t_cmd_redirection			*input_redirections;
	const char					**exec_and_args;
	pid_t						pid;
}	t_command_invocation;



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

// Command execution functions
t_command_invocation	*ast_to_command_invocation(t_ast *ast);
t_cmd_redirection		*create_redirection(t_redirect_type type, const char *file_path);
void					add_redirection(t_cmd_redirection **head, t_cmd_redirection *new_redir);
void					free_redirections(t_cmd_redirection *head);
void					free_command_invocation(t_command_invocation *cmd);
void					print_command_invocation(t_command_invocation *cmd, int level);

#endif