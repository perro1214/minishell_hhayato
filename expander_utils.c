/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:05:47 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:10 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

int	get_var_name_length(char *str)
{
	int	len;

	len = 0;
	while (str[len] && (ft_isalnum(str[len]) || str[len] == '_'))
		len++;
	return (len);
}

static char	*allocate_and_copy_name(char *var_name, int name_len)
{
	char	*name;

	name = malloc(name_len + 1);
	if (!name)
		return (NULL);
	ft_strncpy(name, var_name, name_len);
	name[name_len] = '\0';
	return (name);
}

char	*get_env_value(char *var_name, int name_len, t_env *env_list)
{
	char	*name;
	t_env	*env_node;
	char	*result;

	name = allocate_and_copy_name(var_name, name_len);
	if (!name)
		return (NULL);
	env_node = find_env_node(env_list, name);
	free(name);
	if (!env_node || !env_node->value)
		return (NULL);
	result = malloc(ft_strlen(env_node->value) + 1);
	if (!result)
		return (NULL);
	ft_strcpy(result, env_node->value);
	return (result);
}
