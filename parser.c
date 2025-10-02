#include "lexer_parser.h"

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
