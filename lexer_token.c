/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_token.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:06:06 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:14:24 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

void	add_token(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
		return ;
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

static void	print_token_value(char *type_name, char *value)
{
	if (value)
		printf("Type: %-17s Value: %s\n", type_name, value);
	else
		printf("Type: %-17s Value: %s\n", type_name, "(null)");
}

static char	**get_type_names(void)
{
	static char	*type_names[] = {"EXPANDABLE", "EXPANDABLE_QUOTED", \
"NON_EXPANDABLE", "PIPE", "REDIRECT_IN", "REDIRECT_OUT", \
"REDIRECT_APPEND", "REDIRECT_HEREDOC", "NODE_COMMAND", "EOF_TOKEN"};

	return (type_names);
}

void	print_tokens(t_token *head)
{
	char	**type_names;

	type_names = get_type_names();
	while (head)
	{
		print_token_value(type_names[head->type], head->value);
		head = head->next;
	}
}
