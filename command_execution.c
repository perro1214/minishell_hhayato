/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_execution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:05:00 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:03 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

static t_command_invocation	*convert_simple_command(t_ast *ast, t_env *env_list)
{
	t_command_invocation	*cmd;

	if (ast->type != NODE_COMMAND)
		return (NULL);
	cmd = malloc(sizeof(t_command_invocation));
	if (!cmd)
		return (NULL);
	cmd->redirections = NULL;
	cmd->piped_command = NULL;
	cmd->pid = -1;
	cmd->exec_and_args = create_args_array(ast->left, env_list);
	process_redirections(ast->right, cmd, env_list);
	if (!cmd->exec_and_args || !cmd->exec_and_args[0])
	{
		if (!cmd->redirections)
			free_command_invocation(cmd);
		return (NULL);
	}
	return (cmd);
}

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

static t_command_invocation	*handle_pipe_ast(t_ast *ast, t_env *env_list)
{
	t_command_invocation	*cmd;
	t_command_invocation	*piped_cmd;
	t_command_invocation	*last_cmd;

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

t_command_invocation	*ast_to_command_invocation(t_ast *ast, t_env *env_list)
{
	if (!ast)
		return (NULL);
	if (ast->type == PIPE)
		return (handle_pipe_ast(ast, env_list));
	else if (ast->type == NODE_COMMAND)
		return (convert_simple_command(ast, env_list));
	return (NULL);
}
