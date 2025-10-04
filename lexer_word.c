/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:06:18 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:18 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

int			process_word_part(char *input, int pos, char **temp,
				bool *has_expandable);

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

static void	set_token_type(bool has_expandable_part, t_token_type *type)
{
	if (has_expandable_part)
		*type = EXPANDABLE_QUOTED;
	else
		*type = NON_EXPANDABLE;
}

static int	process_word_loop(char *input, int pos, char **result,
		bool *has_expandable_part)
{
	char	*temp;
	int		new_pos;

	while (input[pos] && !is_whitespace(input[pos])
		&& !is_special_char(input[pos]))
	{
		new_pos = process_word_part(input, pos, &temp, has_expandable_part);
		if (new_pos == -1)
			return (-1);
		*result = append_part(*result, temp);
		if (!*result)
			return (-1);
		pos = new_pos;
	}
	return (pos);
}

int	handle_concatenated_word(char *input, int pos, char **value,
		t_token_type *type)
{
	char	*result;
	bool	has_expandable_part;
	int		final_pos;

	result = ft_strdup("");
	if (!result)
		return (-1);
	has_expandable_part = false;
	final_pos = process_word_loop(input, pos, &result, &has_expandable_part);
	if (final_pos == -1)
		return (-1);
	set_token_type(has_expandable_part, type);
	*value = result;
	return (final_pos);
}
