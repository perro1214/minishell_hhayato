#include "lexer_parser.h"

t_token	*create_token(t_token_type type, const char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
	{
		token->value = malloc(strlen(value) + 1);
		if (!token->value)
		{
			free(token);
			return (NULL);
		}
		strcpy(token->value, value);
	}
	else
		token->value = NULL;
	token->next = NULL;
	return (token);
}

void	add_token(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
		return;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

void	free_tokens(t_token *head)
{
	t_token	*current;
	t_token	*next;

	current = head;
	while (current)
	{
		next = current->next;
		free(current->value);
		free(current);
		current = next;
	}
}

bool	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

bool	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

bool	is_special_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int	skip_whitespace(const char *str, int pos)
{
	while (str[pos] && is_whitespace(str[pos]))
		pos++;
	return (pos);
}

static int	handle_quotes(const char *input, int pos, char **value, t_token_type *type)
{
	char	quote_char;
	int		start;
	int		len;

	quote_char = input[pos];
	start = ++pos;
	while (input[pos] && input[pos] != quote_char)
		pos++;
	if (!input[pos])
	{
		*value = NULL;
		return (-1);
	}
	len = pos - start;
	*value = malloc(len + 1);
	if (!*value)
		return (-1);
	strncpy(*value, &input[start], len);
	(*value)[len] = '\0';
	if (quote_char == '"')
		*type = EXPANDABLE_QUOTED;
	else
		*type = NON_EXPANDABLE;
	return (pos + 1);
}

static int	handle_redirect(const char *input, int pos, char **value, t_token_type *type)
{
	if (input[pos] == '<')
	{
		if (input[pos + 1] == '<')
		{
			*type = REDIRECT_HEREDOC;
			*value = malloc(3);
			if (!*value)
				return (-1);
			strcpy(*value, "<<");
			return (pos + 2);
		}
		else
		{
			*type = REDIRECT_IN;
			*value = malloc(2);
			if (!*value)
				return (-1);
			strcpy(*value, "<");
			return (pos + 1);
		}
	}
	else if (input[pos] == '>')
	{
		if (input[pos + 1] == '>')
		{
			*type = REDIRECT_APPEND;
			*value = malloc(3);
			if (!*value)
				return (-1);
			strcpy(*value, ">>");
			return (pos + 2);
		}
		else
		{
			*type = REDIRECT_OUT;
			*value = malloc(2);
			if (!*value)
				return (-1);
			strcpy(*value, ">");
			return (pos + 1);
		}
	}
	return (-1);
}

static int	handle_pipe(const char *input, int pos, char **value, t_token_type *type)
{
	(void)input;
	*type = PIPE;
	*value = malloc(2);
	if (!*value)
		return (-1);
	strcpy(*value, "|");
	return (pos + 1);
}

static int	handle_word(const char *input, int pos, char **value, t_token_type *type)
{
	int	start;
	int	len;

	start = pos;
	while (input[pos] && !is_whitespace(input[pos]) 
		&& !is_special_char(input[pos]) && !is_quote(input[pos]))
		pos++;
	len = pos - start;
	*value = malloc(len + 1);
	if (!*value)
		return (-1);
	strncpy(*value, &input[start], len);
	(*value)[len] = '\0';
	*type = EXPANDABLE;
	return (pos);
}

t_token	*tokenize(const char *input)
{
	t_token		*head;
	t_token		*token;
	char		*value;
	t_token_type	type;
	int			pos;
	int			new_pos;

	head = NULL;
	pos = 0;
	while (input[pos])
	{
		pos = skip_whitespace(input, pos);
		if (!input[pos])
			break;
		value = NULL;
		if (is_quote(input[pos]))
			new_pos = handle_quotes(input, pos, &value, &type);
		else if (input[pos] == '<' || input[pos] == '>')
			new_pos = handle_redirect(input, pos, &value, &type);
		else if (input[pos] == '|')
			new_pos = handle_pipe(input, pos, &value, &type);
		else
			new_pos = handle_word(input, pos, &value, &type);
		if (new_pos == -1 || !value)
		{
			free_tokens(head);
			return (NULL);
		}
		token = create_token(type, value);
		free(value);
		if (!token)
		{
			free_tokens(head);
			return (NULL);
		}
		add_token(&head, token);
		pos = new_pos;
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

void	print_tokens(t_token *head)
{
	const char	*type_names[] = {
		"EXPANDABLE",
		"EXPANDABLE_QUOTED", 
		"NON_EXPANDABLE",
		"PIPE",
		"REDIRECT_IN",
		"REDIRECT_OUT", 
		"REDIRECT_APPEND",
		"REDIRECT_HEREDOC",
		"EOF_TOKEN"
	};

	while (head)
	{
		if (head->value)
			printf("Type: %-17s Value: %s\n", 
				type_names[head->type], head->value);
		else
			printf("Type: %-17s Value: %s\n", 
				type_names[head->type], "(null)");
		head = head->next;
	}
}