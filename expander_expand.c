/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_expand.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:05:42 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:09 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

static void	expand_status_var(t_expand_ctx *ctx)
{
	char	*status_str;

	status_str = ft_itoa(g_status);
	if (status_str)
	{
		ft_strcpy(&ctx->result[ctx->j], status_str);
		ctx->j += ft_strlen(status_str);
		free(status_str);
	}
	ctx->i += 2;
}

static void	expand_env_var(t_expand_ctx *ctx)
{
	int		var_name_len;
	char	*env_value;
	int		env_value_len;

	var_name_len = get_var_name_length(&ctx->str[ctx->i + 1]);
	env_value = get_env_value(&ctx->str[ctx->i + 1], var_name_len,
			ctx->env_list);
	if (env_value)
	{
		env_value_len = ft_strlen(env_value);
		ft_strncpy(&ctx->result[ctx->j], env_value, env_value_len);
		ctx->j += env_value_len;
		free(env_value);
	}
	ctx->i += var_name_len + 1;
}

static void	expand_loop(char *str, char *result, t_env *env_list)
{
	t_expand_ctx	ctx;

	ctx.str = str;
	ctx.result = result;
	ctx.env_list = env_list;
	ctx.i = 0;
	ctx.j = 0;
	while (ctx.str[ctx.i])
	{
		if (ctx.str[ctx.i] == '$' && ctx.str[ctx.i + 1] == '?')
			expand_status_var(&ctx);
		else if (ctx.str[ctx.i] == '$' && ctx.str[ctx.i + 1]
			&& (ft_isalnum(ctx.str[ctx.i + 1]) || ctx.str[ctx.i + 1] == '_'))
			expand_env_var(&ctx);
		else
		{
			ctx.result[ctx.j] = ctx.str[ctx.i];
			ctx.i++;
			ctx.j++;
		}
	}
	ctx.result[ctx.j] = '\0';
}

char	*expand_variables(char *str, t_env *env_list)
{
	char	*result;

	if (!str)
		return (NULL);
	result = malloc(calculate_expanded_length(str, env_list) + 1);
	if (!result)
		return (NULL);
	expand_loop(str, result, env_list);
	return (result);
}
