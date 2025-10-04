/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_args.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:04:48 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:09:52 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

static int	count_args(t_ast *node)
{
	int	count;

	count = 0;
	while (node)
	{
		if (node->type == EXPANDABLE || node->type == EXPANDABLE_QUOTED
			|| node->type == NON_EXPANDABLE)
			count++;
		node = node->right;
	}
	return (count);
}

static void	free_args_on_error(char **args, int count)
{
	while (--count >= 0)
		free((char *)args[count]);
	free(args);
}

static int	is_valid_arg_type(t_token_type type)
{
	return (type == EXPANDABLE || type == EXPANDABLE_QUOTED
		|| type == NON_EXPANDABLE);
}

static int	process_arg_node(t_ast *node, char **args, int *i, t_env *env_list)
{
	char	*expanded_value;

	if (!is_valid_arg_type(node->type))
		return (1);
	expanded_value = expand_token_value(node->value, node->type, env_list);
	if (!expanded_value)
		return (0);
	args[*i] = expanded_value;
	(*i)++;
	return (1);
}

char	**create_args_array(t_ast *node, t_env *env_list)
{
	char	**args;
	int		count;
	int		i;

	count = count_args(node);
	args = malloc(sizeof(char *) * (count + 1));
	if (!args)
		return (NULL);
	i = 0;
	while (node && i < count)
	{
		if (!process_arg_node(node, args, &i, env_list))
		{
			free_args_on_error(args, i);
			return (NULL);
		}
		node = node->right;
	}
	args[i] = NULL;
	return (args);
}
