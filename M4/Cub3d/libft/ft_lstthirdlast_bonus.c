/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstthirdlast_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 19:37:35 by kfan              #+#    #+#             */
/*   Updated: 2024/10/16 12:23:57 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstthirdlast(t_list *lst)
{
	if (!lst)
		return (NULL);
	while (lst)
	{
		if (lst->next->next->next == NULL)
			return (lst);
		lst = lst->next;
	}
	return (NULL);
}

/*
#include <stdio.h>
int main(void)
{
    t_list s1;
    t_list s2;
	t_list s3;
	t_list *lst;
	
	lst = &s1;
    s1.content = "abc";
    s1.next = &s2;
    s2.content = "def";
	s2.next = &s3;
    s3.content = "123";
    s3.next = NULL;
	ft_lstlast(lst);
    printf("%s\n", (char *)ft_lstlast(lst)->content);
}
*/