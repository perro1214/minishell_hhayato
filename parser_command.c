#include "lexer_parser.h"

static bool	is_word_token(t_token_type type)
{
	return (type == EXPANDABLE || type == EXPANDABLE_QUOTED
		|| type == NON_EXPANDABLE);
}

static bool	handle_redirect(t_parser *parser, t_ast **redirect_head,
		t_ast **last_redirect, t_ast **arg_head)
{
	t_ast	*new_node;

	new_node = parse_redirection(parser);
	if (!new_node)
	{
		free_ast(*arg_head);
		return (false);
	}
	append_node(redirect_head, last_redirect, new_node);
	return (true);
}

static bool	handle_word(t_parser *parser, t_ast **arg_head,
		t_ast **last_arg, t_ast **redirect_head)
{
	t_ast	*new_node;

	new_node = create_ast_node(parser->current->type, parser->current->value);
	if (!new_node)
	{
		free_ast(*arg_head);
		free_ast(*redirect_head);
		return (false);
	}
	append_node(arg_head, last_arg, new_node);
	advance_token(parser);
	return (true);
}

static bool	parse_cmd_loop(t_parser *parser, t_ast **arg_head,
		t_ast **redirect_head, t_ast **last_arg, t_ast **last_redirect)
{
	while (parser->current && parser->current->type != EOF_TOKEN
		&& parser->current->type != PIPE)
	{
		if (is_redirect_token(parser->current->type))
		{
			if (!handle_redirect\
(parser, redirect_head, last_redirect, arg_head))
				return (false);
		}
		else if (is_word_token(parser->current->type))
		{
			if (!handle_word(parser, arg_head, last_arg, redirect_head))
				return (false);
		}
		else
			break ;
	}
	return (true);
}

static void	init_cmd_lists(t_ast **arg_head, t_ast **redirect_head,
		t_ast **last_arg, t_ast **last_redirect)
{
	*arg_head = NULL;
	*redirect_head = NULL;
	*last_arg = NULL;
	*last_redirect = NULL;
}

t_ast	*parse_simple_command(t_parser *parser)
{
	t_ast	*cmd_node;
	t_ast	*arg_list_head;
	t_ast	*redirect_list_head;
	t_ast	*last_arg;
	t_ast	*last_redirect;

	cmd_node = create_ast_node(NODE_COMMAND, NULL);
	if (!cmd_node)
		return (NULL);
	init_cmd_lists(&arg_list_head, &redirect_list_head,
		&last_arg, &last_redirect);
	if (!parse_cmd_loop(parser, &arg_list_head, &redirect_list_head,
			&last_arg, &last_redirect))
		return (free_ast(cmd_node), NULL);
	if (!arg_list_head && !redirect_list_head)
		return (free_ast(cmd_node), NULL);
	cmd_node->left = arg_list_head;
	cmd_node->right = redirect_list_head;
	return (cmd_node);
}

