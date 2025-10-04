/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:05:50 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:10 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

char	*expand_token_value(char *value, t_token_type type, t_env *env_list)
{
	char	*result;

	if (!value)
		return (NULL);
	if (type == NON_EXPANDABLE)
	{
		result = malloc(ft_strlen(value) + 1);
		if (!result)
			return (NULL);
		ft_strcpy(result, value);
		return (result);
	}
	if (type == EXPANDABLE || type == EXPANDABLE_QUOTED)
		return (expand_variables(value, env_list));
	return (NULL);
}
