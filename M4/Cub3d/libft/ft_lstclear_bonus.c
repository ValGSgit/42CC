/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 17:40:22 by kfan              #+#    #+#             */
/*   Updated: 2024/10/14 13:40:17 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*node;
	t_list	*nextnode;

	if (!lst)
		return ;
	node = *lst;
	while (node != NULL)
	{
		if (del)
			del(node->content);
		nextnode = node->next;
		free(node);
		node = nextnode;
	}
	*lst = NULL;
}

/*
#include <stdio.h>
void	printlist(t_list *head)
{
	while (head != NULL)
	{
		printf("%s\n", (char *)head->content);
		head = head->next;
	}
}

void	ft_del(void *s)
{
	free(s);
}

int	main(void)
{
	t_list	*head;
	t_list	*s1;
	t_list	*s2;
	t_list	*s3;

	head = NULL;
	s1 = ft_lstnew(ft_strdup("123"));
	s2 = ft_lstnew(ft_strdup("456"));
	s3 = ft_lstnew(ft_strdup("789"));
	ft_lstadd_front(&head, s3);
	ft_lstadd_front(&head, s2);
	ft_lstadd_front(&head, s1);
	printlist(head);
	ft_lstclear(&s2, ft_del);
	printf("--------------\n");
	s1->next = NULL;
	//use it otherwise it's pointing to some garbage?
	printlist(head);
}
*/