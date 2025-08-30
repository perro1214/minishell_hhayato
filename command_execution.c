#include "lexer_parser.h"
#include <sys/wait.h>
#include <fcntl.h>

static void	create_path(char *dest, const char *dir, const char *cmd)
{
	ft_strlcpy(dest, dir, 1024);
	ft_strlcat(dest, "/", 1024);
	ft_strlcat(dest, cmd, 1024);
}

t_cmd_redirection	*create_redirection(t_redirect_type type, const char *file_path)
{
	t_cmd_redirection	*redir;

	redir = malloc(sizeof(t_cmd_redirection));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->fd = -1;
	redir->next = NULL;
	if (file_path)
	{
		redir->file_path = malloc(ft_strlen(file_path) + 1);
		if (!redir->file_path)
		{
			free(redir);
			return (NULL);
		}
		ft_strlcpy(redir->file_path, file_path, ft_strlen(file_path) + 1);
	}
	else
		redir->file_path = NULL;
	return (redir);
}

void	add_redirection(t_cmd_redirection **head, t_cmd_redirection *new_redir)
{
	t_cmd_redirection	*current;

	if (!head || !new_redir)
		return ;
	if (!*head)
	{
		*head = new_redir;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}

void	free_redirections(t_cmd_redirection *head)
{
	t_cmd_redirection	*current;
	t_cmd_redirection	*next;

	current = head;
	while (current)
	{
		next = current->next;
		free(current->file_path);
		free(current);
		current = next;
	}
}

void	free_command_invocation(t_command_invocation *cmd)
{
	int	i;

	if (!cmd)
		return ;
	free_redirections(cmd->input_redirections);
	free_redirections(cmd->output_redirections);
	if (cmd->exec_and_args)
	{
		i = 0;
		while (cmd->exec_and_args[i])
		{
			free((char *)cmd->exec_and_args[i]);
			i++;
		}
		free(cmd->exec_and_args);
	}
	free_command_invocation(cmd->piped_command);
	free(cmd);
}

static t_redirect_type	token_to_redirect_type(t_token_type type)
{
	if (type == REDIRECT_IN)
		return (REDIR_INPUT);
	else if (type == REDIRECT_OUT)
		return (REDIR_OUTPUT);
	else if (type == REDIRECT_APPEND)
		return (REDIR_APPEND);
	else if (type == REDIRECT_HEREDOC)
		return (REDIR_HEREDOC);
	return (REDIR_INPUT);
}

static int	count_args(t_ast *node)
{
	int	count;

	count = 0;
	while (node)
	{
		if (node->type == EXPANDABLE || node->type == EXPANDABLE_QUOTED || 
			node->type == NON_EXPANDABLE)
			count++;
		else if (is_redirect_token(node->type))
		{
			if (node->right)
				node = node->right;
		}
		node = node->right;
	}
	return (count);
}

static const char	**create_args_array(t_ast *node, t_env *env_list)
{
	const char	**args;
	int			count;
	int			i;
	char		*expanded_value;

	count = count_args(node);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	i = 0;
	while (node && i < count)
	{
		if (node->type == EXPANDABLE || node->type == EXPANDABLE_QUOTED || 
			node->type == NON_EXPANDABLE)
		{
			expanded_value = expand_token_value(node->value, node->type, env_list);
			if (!expanded_value)
			{
				while (--i >= 0)
					free((char *)args[i]);
				free(args);
				return (NULL);
			}
			args[i] = expanded_value;
			i++;
		}
		else if (is_redirect_token(node->type))
		{
			if (node->right)
				node = node->right;
		}
		node = node->right;
	}
	args[i] = NULL;
	return (args);
}

static void	process_redirections(t_ast *node, t_command_invocation *cmd, t_env *env_list)
{
	t_cmd_redirection	*redir;
	t_redirect_type		redir_type;
	char				*expanded_path;

	while (node)
	{
		if (is_redirect_token(node->type))
		{
			redir_type = token_to_redirect_type(node->type);
			if (node->right && node->right->value)
			{
				expanded_path = expand_token_value(node->right->value, node->right->type, env_list);
				if (expanded_path)
				{
					redir = create_redirection(redir_type, expanded_path);
					free(expanded_path);
					if (redir)
					{
						if (redir_type == REDIR_INPUT || redir_type == REDIR_HEREDOC)
							add_redirection(&cmd->input_redirections, redir);
						else
							add_redirection(&cmd->output_redirections, redir);
					}
				}
			}
			if (node->right)
				node = node->right;
		}
		node = node->right;
	}
}

static t_command_invocation	*convert_simple_command(t_ast *ast, t_env *env_list)
{
	t_command_invocation	*cmd;

	cmd = malloc(sizeof(t_command_invocation));
	if (!cmd)
		return (NULL);
	cmd->input_redirections = NULL;
	cmd->output_redirections = NULL;
	cmd->piped_command = NULL;
	cmd->pid = -1;
	
	cmd->exec_and_args = create_args_array(ast, env_list);
	if (!cmd->exec_and_args)
	{
		free(cmd);
		return (NULL);
	}
	
	process_redirections(ast, cmd, env_list);
	
	return (cmd);
}

// 修正箇所　連続パイプの対応
static t_command_invocation	*find_last_command(t_command_invocation *cmd)
{
	t_command_invocation	*current;

	if (!cmd)
		return (NULL);
	current = cmd;
	while (current->piped_command)
		current = current->piped_command;
	return (current);
}

t_command_invocation	*ast_to_command_invocation(t_ast *ast, t_env *env_list)
{
	t_command_invocation	*cmd;
	t_command_invocation	*piped_cmd;
	t_command_invocation	*last_cmd;

	if (!ast)
		return (NULL);
	
	if (ast->type == PIPE)
	{
		cmd = ast_to_command_invocation(ast->left, env_list);
		if (!cmd)
			return (NULL);
		piped_cmd = ast_to_command_invocation(ast->right, env_list);
		if (!piped_cmd)
		{
			free_command_invocation(cmd);
			return (NULL);
		}
		last_cmd = find_last_command(cmd);
		last_cmd->piped_command = piped_cmd;
		return (cmd);
	}

	return (convert_simple_command(ast, env_list));
}







// デバック
static const char	*redirect_type_to_string(t_redirect_type type)
{
	if (type == REDIR_INPUT)
		return ("入力リダイレクト (<)");
	else if (type == REDIR_OUTPUT)
		return ("出力リダイレクト (>)");
	else if (type == REDIR_APPEND)
		return ("追記リダイレクト (>>)");
	else if (type == REDIR_HEREDOC)
		return ("ヒアドキュメント (<<)");
	return ("不明");
}

static void	print_indent(int level)
{
	int	i;

	i = 0;
	while (i < level)
	{
		printf("  ");
		i++;
	}
}

static void	print_redirection_details(t_cmd_redirection *redir, int level, const char *title)
{
	print_indent(level);
	printf("%s:\n", title);
	while (redir)
	{
		print_indent(level + 1);
		printf("• %s → %s\n", 
			redirect_type_to_string(redir->type),
			redir->file_path ? redir->file_path : "(null)");
		redir = redir->next;
	}
}

// command_invocationの内容を表示 デバッグ用
void	print_command_invocation(t_command_invocation *cmd, int level)
{
	int	i;

	if (!cmd)
	{
		print_indent(level);
		printf("NULL コマンド\n");
		return ;
	}
	
	print_indent(level);
	printf("┌─ コマンド %d (addr:%p, piped:%p) ─────────\n", level + 1, (void*)cmd, (void*)cmd->piped_command);
	
	// 実行ファイルと引数を表示
	if (cmd->exec_and_args)
	{
		print_indent(level);
		printf("│ 実行コマンド: ");
		i = 0;
		while (cmd->exec_and_args[i])
		{
			if (i == 0)
				printf("'%s'", cmd->exec_and_args[i]);
			else
				printf(" '%s'", cmd->exec_and_args[i]);
			i++;
		}
		printf("\n");
	}
	
	// 入力リダイレクションを表示
	if (cmd->input_redirections)
	{
		print_indent(level);
		printf("│\n");
		print_redirection_details(cmd->input_redirections, level, "│ 入力リダイレクション");
	}
	
	// 出力リダイレクションを表示
	if (cmd->output_redirections)
	{
		print_indent(level);
		printf("│\n");
		print_redirection_details(cmd->output_redirections, level, "│ 出力リダイレクション");
	}
	
	print_indent(level);
	printf("└────────────────────────────\n");
	
	// パイプされたコマンドを表示
	if (cmd->piped_command)
	{
		print_indent(level);
		printf("    ↓ パイプ (|)\n");
		print_command_invocation(cmd->piped_command, level);
	}
	else
	{
		print_indent(level);
		printf("    (パイプなし - 最後のコマンド)\n");
	}
}

// 修正点　連続パイプの対応

static int	apply_redirections(t_cmd_redirection *redirections)
{
	t_cmd_redirection	*current;
	int					fd;

	current = redirections;
	while (current)
	{
		if (current->type == REDIR_INPUT)
		{
			fd = open(current->file_path, O_RDONLY);
			if (fd == -1)
				return (-1);
			if (dup2(fd, STDIN_FILENO) == -1)
			{
				close(fd);
				return (-1);
			}
			close(fd);
		}
		else if (current->type == REDIR_OUTPUT)
		{
			fd = open(current->file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
				return (-1);
			if (dup2(fd, STDOUT_FILENO) == -1)
			{
				close(fd);
				return (-1);
			}
			close(fd);
		}
		else if (current->type == REDIR_APPEND)
		{
			fd = open(current->file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
				return (-1);
			if (dup2(fd, STDOUT_FILENO) == -1)
			{
				close(fd);
				return (-1);
			}
			close(fd);
		}
		current = current->next;
	}
	return (0);
}

static char	**create_env_array(t_env *env_list)
{
	t_env	*current;
	char	**env_array;
	int		count;
	int		i;
	int		name_len;
	int		value_len;

	count = 0;
	current = env_list;
	while (current)
	{
		count++;
		current = current->next;
	}
	env_array = malloc(sizeof(char *) * (count + 1));
	if (!env_array)
		return (NULL);
	i = 0;
	current = env_list;
	while (current && i < count)
	{
		name_len = ft_strlen(current->name);
		value_len = current->value ? ft_strlen(current->value) : 0;
		env_array[i] = malloc(name_len + value_len + 2);
		if (!env_array[i])
		{
			while (--i >= 0)
				free(env_array[i]);
			free(env_array);
			return (NULL);
		}
		ft_strlcpy(env_array[i], current->name, name_len + 1);
		ft_strlcat(env_array[i], "=", name_len + value_len + 2);
		if (current->value)
			ft_strlcat(env_array[i], current->value, name_len + value_len + 2);
		current = current->next;
		i++;
	}
	env_array[i] = NULL;
	return (env_array);
}

int	execute_simple_command(t_command_invocation *cmd, t_env *env_list)
{
	char	**env_array;
	char	*path;

	if (!cmd || !cmd->exec_and_args || !cmd->exec_and_args[0])
		return (-1);

	if (apply_redirections(cmd->input_redirections) == -1)
		return (-1);
	if (apply_redirections(cmd->output_redirections) == -1)
		return (-1);

	env_array = create_env_array(env_list);
	if (!env_array)
		return (-1);

	path = (char *)cmd->exec_and_args[0];

	execve(path, (char **)cmd->exec_and_args, env_array);
	
	if (path[0] != '/')
	{
		char full_path[1024];
		create_path(full_path, "/bin", path);
		execve(full_path, (char **)cmd->exec_and_args, env_array);
		
		create_path(full_path, "/usr/bin", path);
		execve(full_path, (char **)cmd->exec_and_args, env_array);
	}

	perror("execve");
	

	int i = 0;
	while (env_array[i])
	{
		free(env_array[i]);
		i++;
	}
	free(env_array);
	
	return (-1);
}

static int	save_stdin = -1;
static int	save_stdout = -1;

static void	save_std_fds(void)
{
	if (save_stdin == -1)
		save_stdin = dup(STDIN_FILENO);
	if (save_stdout == -1)
		save_stdout = dup(STDOUT_FILENO);
}

static void	restore_std_fds(void)
{
	if (save_stdin != -1)
	{
		dup2(save_stdin, STDIN_FILENO);
		close(save_stdin);
		save_stdin = -1;
	}
	if (save_stdout != -1)
	{
		dup2(save_stdout, STDOUT_FILENO);
		close(save_stdout);
		save_stdout = -1;
	}
}

int	execute_pipeline(t_command_invocation *cmd, t_env *env_list)
{
	int		pipefd[2];
	pid_t	pid1, pid2;
	int		status1, status2;

	if (!cmd)
		return (-1);

	save_std_fds();

	if (!cmd->piped_command)
	{
		pid1 = fork();
		if (pid1 == 0)
		{
			exit(execute_simple_command(cmd, env_list));
		}
		else if (pid1 > 0)
		{
			waitpid(pid1, &status1, 0);
			restore_std_fds();
			return (WIFEXITED(status1) ? WEXITSTATUS(status1) : -1);
		}
		restore_std_fds();
		return (-1);
	}

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		restore_std_fds();
		return (-1);
	}

	pid1 = fork();
	if (pid1 == 0)
	{
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		exit(execute_simple_command(cmd, env_list));
	}
	else if (pid1 > 0)
	{
		pid2 = fork();
		if (pid2 == 0)
		{
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			exit(execute_pipeline(cmd->piped_command, env_list));
		}
		else if (pid2 > 0)
		{
			close(pipefd[0]);
			close(pipefd[1]);
			waitpid(pid1, &status1, 0);
			waitpid(pid2, &status2, 0);
			restore_std_fds();
			return (WIFEXITED(status2) ? WEXITSTATUS(status2) : -1);
		}
		else
		{
			close(pipefd[0]);
			close(pipefd[1]);
			waitpid(pid1, NULL, 0);
			perror("fork");
			restore_std_fds();
			return (-1);
		}
	}

	close(pipefd[0]);
	close(pipefd[1]);
	perror("fork");
	restore_std_fds();
	return (-1);
}