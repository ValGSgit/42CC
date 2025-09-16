/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 20:15:27 by kfan              #+#    #+#             */
/*   Updated: 2024/10/22 14:25:30 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*ptr;
	t_list	*current;

	ptr = NULL;
	if (!lst)
		return (NULL);
	while (lst)
	{
		current = ft_lstnew(lst->content);
		if (!current)
		{
			ft_lstclear(&ptr, del);
			return (NULL);
		}
		if (f)
			current->content = f(current->content);
		ft_lstadd_back(&ptr, current);
		lst = lst->next;
	}
	return (ptr);
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

void	*ft_test(void *s)
{
	int		i;
	char	*ptr;

	i = 0;
	ptr = s;
	while (ptr[i])
	{
		ptr[i] = ptr[i] + 1;
		i++;
	}
	return (s);
}

int	main(void)
{
	t_list	*head;
	t_list	*s1;
	t_list	*s2;
	t_list	*s3;

	head = NULL;
	s1 = ft_lstnew(ft_strdup("123abc"));
	s2 = ft_lstnew(ft_strdup("456"));
	s3 = ft_lstnew(ft_strdup("5678"));
	ft_lstadd_front(&head, s3);
	ft_lstadd_front(&head, s2);
	ft_lstadd_front(&head, s1);
	printlist(head);
	printf("--------------\n");
	printlist(ft_lstmap(head, ft_test, ft_del));
	printf("--------------\n");
	printlist(head);
}
*/