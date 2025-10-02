#ifndef LEXER_PARSER_H
# define LEXER_PARSER_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <stdbool.h>
# include <sys/types.h>
# include <signal.h>

# include "./minilibft/minilibft.h"

extern volatile sig_atomic_t g_status;

typedef enum e_token_type {
	EXPANDABLE,         // 通常の文字列
	EXPANDABLE_QUOTED,  // ダブルクォート内文字列
	NON_EXPANDABLE,     // シングルクォート内文字列
	PIPE,               // |
	REDIRECT_IN,        // <
	REDIRECT_OUT,       // >
	REDIRECT_APPEND,    // >>
	REDIRECT_HEREDOC,   // <<
	NODE_COMMAND,
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
	t_cmd_redirection			*redirections;
	struct s_command_invocation	*piped_command;
	const char					**exec_and_args;
	pid_t						pid;
}	t_command_invocation;

typedef struct s_env
{
	struct s_env	*prev;
	char			*name;
	char			*value;
	struct s_env	*next;
}	t_env;


typedef struct s_data
{
	t_env					*env_head;
	t_env					*env_tail;
	t_command_invocation	*cmd;
	t_ast					*ast;
	t_token					*tokens;
	char					*input;
}	t_data;

// Lexer functions
t_token		*tokenize(const char *input);
t_token		*create_token(t_token_type type	,const char	*value);
void		add_token(t_token **head, t_token *new_token);
void		free_tokens(t_token *head);
void		print_tokens(t_token *head);

// Parser functions
t_ast		*parse(t_token *tokens);
t_ast		*create_ast_node(t_token_type type, const char *value);
void		free_ast(t_ast *node);
void		print_ast(t_ast *node, int level);
t_ast		*parse_pipeline(t_parser *parser);
t_ast		*parse_simple_command(t_parser *parser);
t_ast		*parse_redirection(t_parser *parser);
t_token		*advance_token(t_parser *parser);
void		append_node(t_ast **list_head, t_ast **list_tail, t_ast *new_node);

// Helper functions
bool		is_redirect_token(t_token_type type);
bool		is_quote(char c);
bool		is_whitespace(char c);
bool		is_special_char(char c);
int			skip_whitespace(const char *str, int pos);

// Command execution functions
t_command_invocation	*ast_to_command_invocation(t_ast *ast, t_env *env_list);
t_cmd_redirection		*create_redirection(t_redirect_type type, const char *file_path);
void					add_redirection(t_cmd_redirection **head, t_cmd_redirection *new_redir);
void					free_redirections(t_cmd_redirection *head);
void					free_command_invocation(t_command_invocation *cmd);
void					print_command_invocation(t_command_invocation *cmd, int level);
const char				**create_args_array(t_ast *node, t_env *env_list);
void					process_redirections(t_ast *node, \
						t_command_invocation *cmd, t_env *env_list);

// 環境変数操作関数
void					free_env_list(t_env *head);
t_env					*make_env_node(char *name, char *value);
void					append_env_node(t_env **head, t_env **tail, t_env *node);
t_env					*find_env_node(t_env *head, const char *name);
int						init_env_list(t_data *data, char *envp[]);

// 変数展開関数
char					*expand_variables(const char *str, t_env *env_list);
char					*expand_token_value(const char *value, t_token_type type,
							t_env *env_list);
int						get_var_name_length(const char *str);
char					*get_env_value(const char *var_name, int name_len,
							t_env *env_list);
int						calculate_expanded_length(const char *str,
							t_env *env_list);

// マルチライン入力処理関数
char					*handle_multiline_input(const char *initial_input);

#endif
