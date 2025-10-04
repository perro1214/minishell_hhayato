/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:06:35 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:21 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

bool		is_word_token(t_token_type type);
bool		handle_cmd_redirect(t_parser *parser, t_cmd_lists *lists);
bool		handle_word(t_parser *parser, t_cmd_lists *lists);

static bool	parse_cmd_loop(t_parser *parser, t_cmd_lists *lists)
{
	while (parser->current && parser->current->type != EOF_TOKEN
		&& parser->current->type != PIPE)
	{
		if (is_redirect_token(parser->current->type))
		{
			if (!handle_cmd_redirect(parser, lists))
				return (false);
		}
		else if (is_word_token(parser->current->type))
		{
			if (!handle_word(parser, lists))
				return (false);
		}
		else
			break ;
	}
	return (true);
}

static void	init_cmd_vars_and_lists(t_cmd_vars *vars, t_cmd_lists *lists)
{
	vars->arg_head = NULL;
	vars->redirect_head = NULL;
	vars->last_arg = NULL;
	vars->last_redirect = NULL;
	lists->arg_head = &vars->arg_head;
	lists->redirect_head = &vars->redirect_head;
	lists->last_arg = &vars->last_arg;
	lists->last_redirect = &vars->last_redirect;
}

t_ast	*parse_simple_command(t_parser *parser)
{
	t_ast		*cmd_node;
	t_cmd_vars	vars;
	t_cmd_lists	lists;

	cmd_node = create_ast_node(NODE_COMMAND, NULL);
	if (!cmd_node)
		return (NULL);
	init_cmd_vars_and_lists(&vars, &lists);
	if (!parse_cmd_loop(parser, &lists))
		return (free_ast(cmd_node), NULL);
	if (!vars.arg_head && !vars.redirect_head)
		return (free_ast(cmd_node), NULL);
	cmd_node->left = vars.arg_head;
	cmd_node->right = vars.redirect_head;
	return (cmd_node);
}
