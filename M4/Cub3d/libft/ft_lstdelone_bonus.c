/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 14:19:13 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 18:44:38 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstdelone(t_list *lst, void (*del)(void *))
{
	if (!del || !lst)
		return ;
	del(lst->content);
	free(lst);
	lst = NULL;
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
	ft_lstdelone(s2, ft_del);
	printf("--------------\n");
	s1->next = s3;
	printlist(head);
}

*/