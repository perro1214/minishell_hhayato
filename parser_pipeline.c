#include "lexer_parser.h"

static t_ast	*handle_pipe_error(t_ast *left, t_ast *pipe_node)
{
	free_ast(left);
	free_ast(pipe_node);
	return (NULL);
}

static t_ast	*build_pipe_node(t_parser *parser, t_ast *left)
{
	t_ast	*pipe_node;
	t_ast	*right;

	pipe_node = create_ast_node(PIPE, "|");
	if (!pipe_node)
		return (free_ast(left), NULL);
	advance_token(parser);
	right = parse_simple_command(parser);
	if (!right)
		return (handle_pipe_error(left, pipe_node));
	pipe_node->left = left;
	pipe_node->right = right;
	return (pipe_node);
}

t_ast	*parse_pipeline(t_parser *parser)
{
	t_ast	*left;

	left = parse_simple_command(parser);
	if (!left)
		return (NULL);
	while (parser->current && parser->current->type == PIPE)
		left = build_pipe_node(parser, left);
	return (left);
}

