#include "lexer_parser.h"
#include <readline/readline.h>

int	handle_redirect\
(const char *input, int pos, char **value, t_token_type *type);

int	handle_concatenated_word\
(const char *input, int pos, char **value, t_token_type *type);

int	handle_pipe(const char *input, int pos, char **value, t_token_type *type)
{
	(void)input;
	*type = PIPE;
	*value = ft_strdup("|");
	if (!*value)
		return (-1);
	return (pos + 1);
}

static int	process_token(const char *input, int pos, char **value,
	t_token_type *type)
{
	if (input[pos] == '<' || input[pos] == '>')
		return (handle_redirect(input, pos, value, type));
	else if (input[pos] == '|')
		return (handle_pipe(input, pos, value, type));
	else
		return (handle_concatenated_word(input, pos, value, type));
}

static t_token	*create_and_add_token(t_token **head, t_token_type type,
	char *value)
{
	t_token	*token;

	token = create_token(type, value);
	free(value);
	if (!token)
	{
		free_tokens(*head);
		return (NULL);
	}
	add_token(head, token);
	return (token);
}

static bool	process_input_token(const char *input, int *pos, t_token **head)
{
	char			*value;
	t_token_type	type;
	int				new_pos;

	value = NULL;
	new_pos = process_token(input, *pos, &value, &type);
	if (new_pos == -1 || !value)
	{
		free_tokens(*head);
		return (false);
	}
	if (!create_and_add_token(head, type, value))
		return (false);
	*pos = new_pos;
	return (true);
}

t_token	*tokenize(const char *input)
{
	t_token	*head;
	t_token	*token;
	int		pos;

	head = NULL;
	pos = 0;
	while (input[pos])
	{
		pos = skip_whitespace(input, pos);
		if (!input[pos])
			break ;
		if (!process_input_token(input, &pos, &head))
			return (NULL);
	}
	token = create_token(EOF_TOKEN, NULL);
	if (!token)
	{
		free_tokens(head);
		return (NULL);
	}
	add_token(&head, token);
	return (head);
}
