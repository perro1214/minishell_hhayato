#include "lexer_parser.h"

void	print_indent(int level);
void	print_left_subtree(t_ast *node, int level);
void	print_right_subtree(t_ast *node, int level);

static const char	**get_type_names(void)
{
	static const char	*type_names[] = {"EXPANDABLE", "EXPANDABLE_QUOTED",
		"NON_EXPANDABLE", "PIPE", "REDIRECT_IN", "REDIRECT_OUT",
		"REDIRECT_APPEND", "REDIRECT_HEREDOC", "NODE_COMMAND", "EOF_TOKEN"};

	return (type_names);
}

static void	print_node_info(t_ast *node, const char **type_names, int level)
{
	print_indent(level);
	if (node->value)
		printf("%s: %s\n", type_names[node->type], node->value);
	else
		printf("%s: %s\n", type_names[node->type], "(null)");
}

void	print_ast(t_ast *node, int level)
{
	const char	**type_names;

	if (!node)
		return ;
	type_names = get_type_names();
	print_node_info(node, type_names, level);
	print_left_subtree(node, level);
	print_right_subtree(node, level);
}
