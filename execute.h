#ifndef EXECUTE_H
# define EXECUTE_H

# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <errno.h>
# include <sys/stat.h>

# include "lexer_parser.h"
# include "minilibft.h"
# include "builtin.h"

# define STATUS_COMMAND_NOT_FOUND 127

extern volatile sig_atomic_t	g_status;

typedef enum e_proctype
{
	PARENTS,
	CHILDREN
}	t_proctype;

typedef struct s_fds
{
	int	in_fd;
	int	out_fd;
	int	pipe_fd[2];
}	t_fds;

typedef struct s_child_status
{
	pid_t	pid;
	int		status;
}	t_child_status;

typedef enum e_execve_error
{
	COMMAND_NOT_FOUND,
	COMMAND_NOT_FOUND_PATH,
	IS_A_DIRECTORY,
	PERMISSION_DENIED,
	EXECVE_ERROR
}	t_execve_error;

typedef struct s_signal
{
	struct sigaction	sa_ign;
	struct sigaction	sa_old_int;
	struct sigaction	sa_old_quit;
}	t_signal;

int		apply_redirections(t_command_invocation *cmd);
char	*find_command_path(char *cmd, t_env *env_list);

t_cmd_redirection	*create_redirection(t_redirect_type type, \
					const char *file_path);
void				add_redirection(t_cmd_redirection **head, \
					t_cmd_redirection *new_redir);
void				free_redirections(t_cmd_redirection *head);
void				free_command_invocation(t_command_invocation *cmd);
const char			**create_args_array(t_ast *node, t_env *env_list);
void				process_redirections(t_ast *node, \
					t_command_invocation *cmd, t_env *env_list);
t_command_invocation	*ast_to_command_invocation(t_ast *ast, \
					t_env *env_list);

void	set_signal_handler(void);
void	set_parent_signal_handlers(void);

void	prepro_execute_child_process(t_fds *fds, \
		t_command_invocation *current_cmd, t_data *data);

int		execute_builtin(t_command_invocation *cmd, t_data *data);

char	*readline_input(void);
int		execute_ast(t_command_invocation *cmd_list, t_data *data);

pid_t	*allocate_pid_array(t_command_invocation *cmd_list, int *cmd_count);
int		check_status(int status);
void	wait_and_collect_statuses(int cmd_count, \
		pid_t *pids, t_child_status *statuses);
int		put_fork_error(pid_t *pids, t_fds *fds);
void	ft_close_fd(t_fds *fds, t_proctype type);

void	set_shlvl(t_data *data);

pid_t	run_pipeline_commands(t_command_invocation *cmd_list, \
				pid_t *pids, t_data *data, t_fds *fds);

int		execute_pipeline(t_command_invocation *cmd_list, t_data *data);

int		preprocess_heredocs(t_command_invocation *cmd_list);

void	prepro_ft_put_error(char *command, \
			char *path, char **current_envp);
void	ft_put_error(char *command, char *path, \
							char **env_array, t_execve_error type);

#endif
