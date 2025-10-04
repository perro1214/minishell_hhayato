/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_ast.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:06:26 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:19 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

static char	*allocate_value(char *value, t_ast *node)
{
	char	*new_value;

	new_value = malloc(ft_strlen(value) + 1);
	if (!new_value)
	{
		free(node);
		return (NULL);
	}
	ft_strcpy(new_value, value);
	return (new_value);
}

t_ast	*create_ast_node(t_token_type type, char *value)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (!node)
		return (NULL);
	node->type = type;
	if (value)
		node->value = allocate_value(value, node);
	else
		node->value = NULL;
	if (value && !node->value)
		return (NULL);
	node->left = NULL;
	node->right = NULL;
	return (node);
}

void	free_ast(t_ast *node)
{
	if (!node)
		return ;
	free_ast(node->left);
	free_ast(node->right);
	free(node->value);
	free(node);
}

bool	is_redirect_token(t_token_type type)
{
	return (type == REDIRECT_IN || type == REDIRECT_OUT
		|| type == REDIRECT_APPEND || type == REDIRECT_HEREDOC);
}
