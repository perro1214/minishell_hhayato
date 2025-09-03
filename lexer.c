#include "lexer_parser.h"
#include <readline/readline.h>

t_token	*create_token(t_token_type type, const char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
	{
		token->value = ft_strdup(value);
		if (!token->value)
		{
			free(token);
			return (NULL);
		}
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
	ft_strlcpy(*value, &input[start], len + 1);
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
			*value = ft_strdup("<<");
			if (!*value)
				return (-1);
			return (pos + 2);
		}
		else
		{
			*type = REDIRECT_IN;
			*value = ft_strdup("<");
			if (!*value)
				return (-1);
			return (pos + 1);
		}
	}
	else if (input[pos] == '>')
	{
		if (input[pos + 1] == '>')
		{
			*type = REDIRECT_APPEND;
			*value = ft_strdup(">>");
			if (!*value)
				return (-1);
			return (pos + 2);
		}
		else
		{
			*type = REDIRECT_OUT;
			*value = ft_strdup(">");
			if (!*value)
				return (-1);
			return (pos + 1);
		}
	}
	return (-1);
}

static int	handle_pipe(const char *input, int pos, char **value, t_token_type *type)
{
	(void)input;
	*type = PIPE;
	*value = ft_strdup("|");
	if (!*value)
		return (-1);
	return (pos + 1);
}

static int	handle_concatenated_word(const char *input, int pos, char **value, t_token_type *type)
{
	char	*result;
	char	*temp;
	char	*new_result;
	int		start_pos;
	t_token_type temp_type;
	int		new_pos;
	bool	has_expandable_part;

	result = ft_strdup("");
	if (!result)
		return (-1);
	has_expandable_part = false;

	while (input[pos] && !is_whitespace(input[pos]) && !is_special_char(input[pos]))
	{
		temp = NULL;
		if (is_quote(input[pos]))
		{
			new_pos = handle_quotes(input, pos, &temp, &temp_type);
			if (new_pos == -1 || !temp)
			{
				free(result);
				return (-1);
			}
			if (temp_type == EXPANDABLE_QUOTED)
				has_expandable_part = true;
		}
		else
		{
			start_pos = pos;
			while (input[pos] && !is_whitespace(input[pos]) 
				&& !is_special_char(input[pos]) && !is_quote(input[pos]))
				pos++;
			temp = ft_substr(input, start_pos, pos - start_pos);
			if (!temp)
			{
				free(result);
				return (-1);
			}
			new_pos = pos;
			has_expandable_part = true;
		}

		new_result = ft_strjoin(result, temp);
		if (!new_result)
		{
			free(result);
			free(temp);
			return (-1);
		}
		free(result);
		free(temp);
		result = new_result;
		pos = new_pos;
	}

	if (has_expandable_part)
		*type = EXPANDABLE_QUOTED;
	else
		*type = NON_EXPANDABLE;

	*value = result;
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
		if (input[pos] == '<' || input[pos] == '>')
			new_pos = handle_redirect(input, pos, &value, &type);
		else if (input[pos] == '|')
			new_pos = handle_pipe(input, pos, &value, &type);
		else
			new_pos = handle_concatenated_word(input, pos, &value, &type);
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

// 未閉じのクォートをチェックする関数 追加
static bool	has_unclosed_quote(const char *str)
{
	bool	in_single_quote;
	bool	in_double_quote;
	int		i;

	in_single_quote = false;
	in_double_quote = false;
	i = 0;
	
	while (str[i])
	{
		if (str[i] == '\'' && !in_double_quote)
			in_single_quote = !in_single_quote;
		else if (str[i] == '"' && !in_single_quote)
			in_double_quote = !in_double_quote;
		i++;
	}
	
	return (in_single_quote || in_double_quote);
}

// マルチライン入力処理関数　追加
char	*handle_multiline_input(const char *initial_input)
{
	char	*result;
	char	*line;
	char	*temp;

	if (!initial_input)
		return (NULL);
		
	result = ft_strdup(initial_input);
	if (!result)
		return (NULL);
	while (has_unclosed_quote(result))
	{
		line = readline("dquote> ");
		if (!line)
		{
			free(result);
			return (NULL);
		}
		if (strlen(line) == 0)
		{
			free(line);
			continue;
		}
		temp = ft_strjoin(result, "\n");
		if (!temp)
		{
			free(result);
			free(line);
			return (NULL);
		}
		free(result);
		
		result = ft_strjoin(temp, line);
		free(temp);
		free(line);
		
		if (!result)
			return (NULL);
	}
	
	return (result);
}