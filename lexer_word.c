#include "lexer_parser.h"

int	handle_quotes(const char *input, int pos, char **value, t_token_type *type);

static int	process_quoted_part(const char *input, int pos, char **temp,
	bool *has_expandable)
{
	t_token_type	temp_type;
	int				new_pos;

	new_pos = handle_quotes(input, pos, temp, &temp_type);
	if (new_pos == -1 || !*temp)
		return (-1);
	if (temp_type == EXPANDABLE_QUOTED)
		*has_expandable = true;
	return (new_pos);
}

static int	process_unquoted_part(const char *input, int pos, char **temp)
{
	int	start_pos;

	start_pos = pos;
	while (input[pos] && !is_whitespace(input[pos])
		&& !is_special_char(input[pos]) && !is_quote(input[pos]))
		pos++;
	*temp = ft_substr(input, start_pos, pos - start_pos);
	if (!*temp)
		return (-1);
	return (pos);
}

static int	process_word_part(const char *input, int pos, char **temp,
	bool *has_expandable)
{
	int	new_pos;

	*temp = NULL;
	if (is_quote(input[pos]))
	{
		new_pos = process_quoted_part(input, pos, temp, has_expandable);
		if (new_pos == -1)
			return (-1);
	}
	else
	{
		new_pos = process_unquoted_part(input, pos, temp);
		if (new_pos == -1)
			return (-1);
		*has_expandable = true;
	}
	return (new_pos);
}

static char	*append_part(char *result, char *temp)
{
	char	*new_result;

	new_result = ft_strjoin(result, temp);
	if (!new_result)
	{
		free(result);
		free(temp);
		return (NULL);
	}
	free(result);
	free(temp);
	return (new_result);
}

int	handle_concatenated_word\
(const char *input, int pos, char **value,t_token_type *type)
{
	char	*result;
	char	*temp;
	int		new_pos;
	bool	has_expandable_part;

	result = ft_strdup("");
	if (!result)
		return (-1);
	has_expandable_part = false;
	while (input[pos] && !is_whitespace(input[pos])
		&& !is_special_char(input[pos]))
	{
		new_pos = process_word_part(input, pos, &temp, &has_expandable_part);
		if (new_pos == -1)
			return (-1);
		result = append_part(result, temp);
		if (!result)
			return (-1);
		pos = new_pos;
	}
	if (has_expandable_part)
		*type = EXPANDABLE_QUOTED;
	else
		*type = NON_EXPANDABLE;
	*value = result;
	return (pos);
}

