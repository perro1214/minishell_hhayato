#include "lexer_parser.h"

static void	expand_status_var(char *result, int *i, int *j)
{
	char	*status_str;

	status_str = ft_itoa(g_status);
	if (status_str)
	{
		ft_strcpy(&result[*j], status_str);
		*j += ft_strlen(status_str);
		free(status_str);
	}
	*i += 2;
}

static void	expand_env_var\
(const char *str, char *result, int *i, int *j, t_env	*env_list)
{
	int		var_name_len;
	char	*env_value;
	int		env_value_len;

	var_name_len = get_var_name_length(&str[*i + 1]);
	env_value = get_env_value(&str[*i + 1], var_name_len, env_list);
	if (env_value)
	{
		env_value_len = ft_strlen(env_value);
		ft_strncpy(&result[*j], env_value, env_value_len);
		*j += env_value_len;
		free(env_value);
	}
	*i += var_name_len + 1;
}

static void	expand_loop(const char *str, char *result, t_env *env_list)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] == '?')
			expand_status_var(result, &i, &j);
		else if (str[i] == '$' && str[i + 1]
			&& (ft_isalnum(str[i + 1]) || str[i + 1] == '_'))
			expand_env_var(str, result, &i, &j, env_list);
		else
		{
			result[j] = str[i];
			i++;
			j++;
		}
	}
	result[j] = '\0';
}

char	*expand_variables(const char *str, t_env *env_list)
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

