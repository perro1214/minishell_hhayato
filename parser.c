#include "lexer_parser.h"

t_ast	*create_ast_node(t_token_type type, const char *value)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (!node)
		return (NULL);
	node->type = type;
	if (value)
	{
		node->value = malloc(strlen(value) + 1);
		if (!node->value)
		{
			free(node);
			return (NULL);
		}
		strcpy(node->value, value);
	}
	else
		node->value = NULL;
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

static t_token	*advance_token(t_parser *parser)
{
	if (parser->current && parser->current->type != EOF_TOKEN)
		parser->current = parser->current->next;
	return (parser->current);
}

static t_ast	*parse_redirection(t_parser *parser)
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
	if (parser->current->type != EXPANDABLE
		&& parser->current->type != EXPANDABLE_QUOTED)
	{
		free_ast(redirect_node);
		return (NULL);
	}
	file_node = create_ast_node(parser->current->type, parser->current->value);
	if (!file_node)
	{
		free_ast(redirect_node);
		return (NULL);
	}
	redirect_node->right = file_node;
	advance_token(parser);
	return (redirect_node);
}

static t_ast	*parse_simple_command(t_parser *parser)
{
	t_ast	*cmd_node;
	t_ast	*arg_node;
	t_ast	*redirect_node;
	t_ast	*last_arg;

	cmd_node = NULL;
	last_arg = NULL;
	while (parser->current && parser->current->type != EOF_TOKEN
		&& parser->current->type != PIPE)
	{
		if (is_redirect_token(parser->current->type))
		{
			redirect_node = parse_redirection(parser);
			if (!redirect_node)
			{
				free_ast(cmd_node);
				return (NULL);
			}
			if (!cmd_node)
				cmd_node = redirect_node;
			else
			{
				if (last_arg)
					last_arg->right = redirect_node;
				else
					cmd_node->left = redirect_node;
				last_arg = redirect_node;
			}
		}
		else if (parser->current->type == EXPANDABLE
			|| parser->current->type == EXPANDABLE_QUOTED
			|| parser->current->type == NON_EXPANDABLE)
		{
			arg_node = create_ast_node(parser->current->type,
					parser->current->value);
			if (!arg_node)
			{
				free_ast(cmd_node);
				return (NULL);
			}
			if (!cmd_node)
			{
				cmd_node = arg_node;
				last_arg = arg_node;
			}
			else
			{
				last_arg->right = arg_node;
				last_arg = arg_node;
			}
			advance_token(parser);
		}
		else
			break ;
	}
	return (cmd_node);
}

static t_ast	*parse_pipeline(t_parser *parser)
{
	t_ast	*left;
	t_ast	*pipe_node;
	t_ast	*right;

	left = parse_simple_command(parser);
	if (!left)
		return (NULL);
	while (parser->current && parser->current->type == PIPE)
	{
		pipe_node = create_ast_node(PIPE, "|");
		if (!pipe_node)
		{
			free_ast(left);
			return (NULL);
		}
		advance_token(parser);
		right = parse_simple_command(parser);
		if (!right)
		{
			free_ast(left);
			free_ast(pipe_node);
			return (NULL);
		}
		pipe_node->left = left;
		pipe_node->right = right;
		left = pipe_node;
	}
	return (left);
}

t_ast	*parse(t_token *tokens)
{
	t_parser	parser;
	t_ast		*ast;

	if (!tokens)
		return (NULL);
	parser.tokens = tokens;
	parser.current = tokens;
	if (parser.current->type == EOF_TOKEN)
		return (NULL);
	ast = parse_pipeline(&parser);
	return (ast);
}

void	print_ast(t_ast *node, int level)
{
	const char *type_names[] = {"EXPANDABLE", "EXPANDABLE_QUOTED",
		"NON_EXPANDABLE", "PIPE", "REDIRECT_IN", "REDIRECT_OUT",
		"REDIRECT_APPEND", "REDIRECT_HEREDOC", "EOF_TOKEN"};
	int i;

	if (!node)
		return ;
	i = 0;
	while (i < level)
	{
		printf("  ");
		i++;
	}
	if (node->value)
		printf("%s: %s\n", type_names[node->type], node->value);
	else
		printf("%s: %s\n", type_names[node->type], "(null)");
	if (node->left)
	{
		i = 0;
		while (i < level)
		{
			printf("  ");
			i++;
		}
		printf("├─ left:\n");
		print_ast(node->left, level + 1);
	}
	if (node->right)
	{
		i = 0;
		while (i < level)
		{
			printf("  ");
			i++;
		}
		printf("└─ right:\n");
		print_ast(node->right, level + 1);
	}
}