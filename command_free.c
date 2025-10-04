/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_free.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:05:04 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:04 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

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

static void	free_exec_and_args(char **exec_and_args)
{
	int	i;

	if (!exec_and_args)
		return ;
	i = 0;
	while (exec_and_args[i])
	{
		free((char *)exec_and_args[i]);
		i++;
	}
	free(exec_and_args);
}

void	free_command_invocation(t_command_invocation *cmd)
{
	if (!cmd)
		return ;
	free_redirections(cmd->redirections);
	free_exec_and_args(cmd->exec_and_args);
	free_command_invocation(cmd->piped_command);
	free(cmd);
}
