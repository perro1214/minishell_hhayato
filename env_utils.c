#include "lexer_parser.h"

void	free_env_list(t_env *head)
{
	t_env	*tmp;

	while (head)
	{
		tmp = head->next;
		free(head->name);
		free(head->value);
		free(head);
		head = tmp;
	}
}

t_env	*make_env_node(char *name, char *value)
{
	t_env	*node;

	node = (t_env *)malloc(sizeof(t_env));
	if (!node)
		return (NULL);
	node->name = name;
	node->value = value;
	node->prev = NULL;
	node->next = NULL;
	return (node);
}

void	append_env_node(t_env **head, t_env **tail, t_env *node)
{
	if (!*head)
		*head = node;
	else
	{
		(*tail)->next = node;
		node->prev = *tail;
	}
	*tail = node;
}

t_env	*find_env_node(t_env *head, const char *name)
{
	while (head)
	{
		if (ft_strncmp(head->name, name, ft_strlen(name) + 1) == 0)
			return (head);
		head = head->next;
	}
	return (NULL);
}

static char	*ft_strndup(const char *s, size_t n)
{
	char	*result;
	size_t	i;

	result = malloc(n + 1);
	if (!result)
		return (NULL);
	i = 0;
	while (i < n && s[i])
	{
		result[i] = s[i];
		i++;
	}
	result[i] = '\0';
	return (result);
}


static t_env	*split_env(char *envp)
{
	char	*eq;
	char	*name;
	char	*value;
	t_env	*node;

	eq = ft_strchr(envp, '=');
	if (!eq)
		return (NULL);
	name = ft_strndup(envp, eq - envp);
	value = ft_strdup(eq + 1);
	if (!name || !value)
	{
		free(name);
		free(value);
		return (NULL);
	}
	node = make_env_node(name, value);
	if (!node)
	{
		free(name);
		free(value);
	}
	return (node);
}

int	init_env_list(t_data *data, char *envp[])
{
	t_env	*node;
	size_t	i;

	data->env_head = NULL;
	data->env_tail = NULL;
	i = 0;
	while (envp[i])
	{
		node = split_env(envp[i]);
		if (!node)
		{
			free_env_list(data->env_head);
			return (-1);
		}
		append_env_node(&data->env_head, &data->env_tail, node);
		i++;
	}
	return (0);
}