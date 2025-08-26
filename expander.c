#include "lexer_parser.h"

static int	get_var_name_length(const char *str)
{
	int	len;

	len = 0;
	while (str[len] && (isalnum(str[len]) || str[len] == '_'))
		len++;
	return (len);
}

static char	*get_env_value(const char *var_name, int name_len, t_env *env_list)
{
	char	*name;
	t_env	*env_node;
	char	*result;

	name = malloc(name_len + 1);
	if (!name)
		return (NULL);
	strncpy(name, var_name, name_len);
	name[name_len] = '\0';
	env_node = find_env_node(env_list, name);
	free(name);
	if (!env_node || !env_node->value)
		return (malloc(1));
	result = malloc(strlen(env_node->value) + 1);
	if (!result)
		return (NULL);
	strcpy(result, env_node->value);
	return (result);
}

static int	calculate_expanded_length(const char *str, t_env *env_list)
{
	int		total_len;
	int		i;
	int		var_name_len;
	char	*env_value;

	total_len = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] && (isalnum(str[i + 1]) || str[i + 1] == '_'))
		{
			var_name_len = get_var_name_length(&str[i + 1]);
			env_value = get_env_value(&str[i + 1], var_name_len, env_list);
			if (env_value)
			{
				total_len += strlen(env_value);
				free(env_value);
			}
			i += var_name_len + 1;
		}
		else
		{
			total_len++;
			i++;
		}
	}
	return (total_len);
}

char	*expand_variables(const char *str, t_env *env_list)
{
	char	*result;
	char	*env_value;
	int		i;
	int		j;
	int		var_name_len;
	int		env_value_len;

	if (!str)
		return (NULL);
	result = malloc(calculate_expanded_length(str, env_list) + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] && (isalnum(str[i + 1]) || str[i + 1] == '_'))
		{
			var_name_len = get_var_name_length(&str[i + 1]);
			env_value = get_env_value(&str[i + 1], var_name_len, env_list);
			if (env_value)
			{
				env_value_len = strlen(env_value);
				strncpy(&result[j], env_value, env_value_len);
				j += env_value_len;
				free(env_value);
			}
			i += var_name_len + 1;
		}
		else
		{
			result[j] = str[i];
			i++;
			j++;
		}
	}
	result[j] = '\0';
	return (result);
}

char	*expand_token_value(const char *value, t_token_type type, t_env *env_list)
{
	if (!value)
		return (NULL);
	if (type == NON_EXPANDABLE)
	{
		char *result = malloc(strlen(value) + 1);
		if (!result)
			return (NULL);
		strcpy(result, value);
		return (result);
	}
	if (type == EXPANDABLE || type == EXPANDABLE_QUOTED)
		return (expand_variables(value, env_list));
	return (NULL);
}