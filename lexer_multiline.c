/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_multiline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:05:59 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:12 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"
#include <readline/readline.h>

static bool	has_unclosed_quote(char *str)
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

static char	*join_multiline(char *result, char *line)
{
	char	*temp;
	char	*new_result;

	temp = ft_strjoin(result, "\n");
	if (!temp)
	{
		free(result);
		free(line);
		return (NULL);
	}
	free(result);
	new_result = ft_strjoin(temp, line);
	free(temp);
	free(line);
	return (new_result);
}

static char	*read_next_line(char *result)
{
	char	*line;

	line = readline("dquote> ");
	if (!line)
	{
		free(result);
		return (NULL);
	}
	if (ft_strlen(line) == 0)
	{
		free(line);
		return (result);
	}
	return (join_multiline(result, line));
}

char	*handle_multiline_input(char *initial_input)
{
	char	*result;

	if (!initial_input)
		return (NULL);
	result = ft_strdup(initial_input);
	if (!result)
		return (NULL);
	while (has_unclosed_quote(result))
	{
		result = read_next_line(result);
		if (!result)
			return (NULL);
	}
	return (result);
}
