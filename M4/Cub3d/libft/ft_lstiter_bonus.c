/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 19:54:56 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 18:44:59 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(void *))
{
	if (!f || !lst)
		return ;
	while (lst != NULL)
	{
		f(lst->content);
		lst = lst->next;
	}
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

void	ft_test(void *s)
{
    int     i;
    char    *ptr;

    i = 0;
    ptr = s;
	while (ptr[i])
    {
        ptr[i] = '0';
        i++;
    }
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
	ft_lstiter(s2, ft_test);
	printf("--------------\n");
	printlist(head);
}
*/