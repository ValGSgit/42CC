/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 17:44:45 by kfan              #+#    #+#             */
/*   Updated: 2024/10/18 14:07:41 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_lstsize(t_list *lst)
{
	int	count;

	count = 0;
	if (!lst)
		return (0);
	while (lst)
	{
		lst = lst->next;
		count++;
	}
	return (count);
}

/*
#include <stdio.h>

int	main(void)
{
	t_list	s1;
	t_list	s2;
	t_list	*lst;

	s1.content = "abc";
	s1.next = &s2;
	s2.content = "def";
	s2.next = NULL;
	lst = &s1;
	printf("%p\n", &lst);
	printf("%p\n", s1.next);
	printf("%p\n", s2.next);
	printf("size:%d\n", ft_lstsize(lst));
	printf("%p\n", &lst);
	printf("%p\n", s1.next);
	printf("%p\n", s2.next);
	printf("%s\n", (char *)s1.content);
	printf("NULL?%sNULL?\n", (char *)s2.content);
}
*/