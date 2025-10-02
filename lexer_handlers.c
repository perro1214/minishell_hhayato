#include "lexer_parser.h"

static int	extract_quoted_value\
(const char *input, int start, int end, char **value)
{
	int	len;

	len = end - start;
	*value = malloc(len + 1);
	if (!*value)
		return (-1);
	ft_strlcpy(*value, &input[start], len + 1);
	return (0);
}

int	handle_quotes(const char *input, int pos, char **value, t_token_type *type)
{
	char	quote_char;
	int		start;

	quote_char = input[pos];
	start = ++pos;
	while (input[pos] && input[pos] != quote_char)
		pos++;
	if (!input[pos])
	{
		*value = NULL;
		return (-1);
	}
	if (extract_quoted_value(input, start, pos, value) == -1)
		return (-1);
	if (quote_char == '"')
		*type = EXPANDABLE_QUOTED;
	else
		*type = NON_EXPANDABLE;
	return (pos + 1);
}

static int	handle_input_redirect(const char *input, int pos, char **value)
{
	if (input[pos + 1] == '<')
	{
		*value = ft_strdup("<<");
		if (!*value)
			return (-1);
		return (pos + 2);
	}
	else
	{
		*value = ft_strdup("<");
		if (!*value)
			return (-1);
		return (pos + 1);
	}
}

static int	handle_output_redirect(const char *input, int pos, char **value)
{
	if (input[pos + 1] == '>')
	{
		*value = ft_strdup(">>");
		if (!*value)
			return (-1);
		return (pos + 2);
	}
	else
	{
		*value = ft_strdup(">");
		if (!*value)
			return (-1);
		return (pos + 1);
	}
}

int	handle_redirect\
(const char *input, int pos, char **value, t_token_type *type)
{
	int	result;

	if (input[pos] == '<')
	{
		result = handle_input_redirect(input, pos, value);
		if (result == pos + 2)
			*type = REDIRECT_HEREDOC;
		else
			*type = REDIRECT_IN;
		return (result);
	}
	else if (input[pos] == '>')
	{
		result = handle_output_redirect(input, pos, value);
		if (result == pos + 2)
			*type = REDIRECT_APPEND;
		else
			*type = REDIRECT_OUT;
		return (result);
	}
	return (-1);
}

