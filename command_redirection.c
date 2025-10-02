#include "lexer_parser.h"

static char	*allocate_and_copy_path(const char *file_path)
{
	char	*path;

	path = malloc(ft_strlen(file_path) + 1);
	if (!path)
		return (NULL);
	ft_strlcpy(path, file_path, ft_strlen(file_path) + 1);
	return (path);
}

static void	init_redirection(t_cmd_redirection *redir, t_redirect_type type)
{
	redir->type = type;
	redir->fd = -1;
	redir->next = NULL;
	redir->file_path = NULL;
}

static int	set_file_path(t_cmd_redirection *redir, const char *file_path)
{
	if (!file_path)
		return (1);
	redir->file_path = allocate_and_copy_path(file_path);
	if (!redir->file_path)
		return (0);
	return (1);
}

t_cmd_redirection	*create_redirection(t_redirect_type type, \
const char *file_path)
{
	t_cmd_redirection	*redir;

	redir = malloc(sizeof(t_cmd_redirection));
	if (!redir)
		return (NULL);
	init_redirection(redir, type);
	if (!set_file_path(redir, file_path))
	{
		free(redir);
		return (NULL);
	}
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

