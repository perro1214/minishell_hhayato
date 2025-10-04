/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word_process.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:06:15 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:17 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

int	handle_quotes(char *input, int pos, char **value, t_token_type *type);

int	process_quoted_part(char *input, int pos, char **temp, bool *has_expandable)
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

int	process_unquoted_part(char *input, int pos, char **temp)
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

int	process_word_part(char *input, int pos, char **temp, bool *has_expandable)
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
