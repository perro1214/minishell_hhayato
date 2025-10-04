/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_process_redir.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:05:08 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:05 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

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

static void	process_single_redirection(t_ast *node, t_command_invocation *cmd,
		t_env *env_list)
{
	t_cmd_redirection	*redir;
	t_redirect_type		redir_type;
	char				*expanded_path;

	if (!is_redirect_token(node->type))
		return ;
	redir_type = token_to_redirect_type(node->type);
	if (node->left && node->left->value)
	{
		expanded_path = expand_token_value(node->left->value, node->left->type,
				env_list);
		if (expanded_path)
		{
			redir = create_redirection(redir_type, expanded_path);
			free(expanded_path);
			if (redir)
				add_redirection(&cmd->redirections, redir);
		}
	}
}

void	process_redirections(t_ast *node, t_command_invocation *cmd,
		t_env *env_list)
{
	while (node)
	{
		process_single_redirection(node, cmd, env_list);
		node = node->right;
	}
}
