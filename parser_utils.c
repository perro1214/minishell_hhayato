/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:06:51 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:25 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

t_token	*advance_token(t_parser *parser)
{
	if (parser->current && parser->current->type != EOF_TOKEN)
		parser->current = parser->current->next;
	return (parser->current);
}

void	append_node(t_ast **list_head, t_ast **list_tail, t_ast *new_node)
{
	if (*list_head == NULL)
	{
		*list_head = new_node;
		*list_tail = new_node;
	}
	else
	{
		(*list_tail)->right = new_node;
		*list_tail = new_node;
	}
}

static t_ast	*create_file_node(t_parser *parser, t_ast *redirect_node)
{
	t_ast	*file_node;

	file_node = create_ast_node(parser->current->type, parser->current->value);
	if (!file_node)
	{
		free_ast(redirect_node);
		return (NULL);
	}
	return (file_node);
}

static bool	is_valid_file_token(t_token_type type)
{
	return (type == EXPANDABLE || type == EXPANDABLE_QUOTED
		|| type == NON_EXPANDABLE);
}

t_ast	*parse_redirection(t_parser *parser)
{
	t_ast	*redirect_node;
	t_ast	*file_node;

	if (!is_redirect_token(parser->current->type))
		return (NULL);
	redirect_node = create_ast_node(parser->current->type,
			parser->current->value);
	if (!redirect_node)
		return (NULL);
	advance_token(parser);
	if (!is_valid_file_token(parser->current->type))
		return (free_ast(redirect_node), NULL);
	file_node = create_file_node(parser, redirect_node);
	if (!file_node)
		return (NULL);
	redirect_node->left = file_node;
	advance_token(parser);
	return (redirect_node);
}
