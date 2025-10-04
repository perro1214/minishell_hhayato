/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_print_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hhayato <hhayato@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 14:06:43 by hhayato           #+#    #+#             */
/*   Updated: 2025/10/04 14:10:23 by hhayato          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_parser.h"

void	print_indent(int level)
{
	int	i;

	i = 0;
	while (i < level)
	{
		printf("  ");
		i++;
	}
}

void	print_left_subtree(t_ast *node, int level)
{
	if (!node->left)
		return ;
	print_indent(level);
	printf("├─ left:\n");
	print_ast(node->left, level + 1);
}

void	print_right_subtree(t_ast *node, int level)
{
	if (!node->right)
		return ;
	print_indent(level);
	printf("└─ right:\n");
	print_ast(node->right, level + 1);
}
