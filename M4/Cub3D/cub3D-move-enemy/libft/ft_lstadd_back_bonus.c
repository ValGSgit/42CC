/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 19:55:07 by kfan              #+#    #+#             */
/*   Updated: 2024/09/19 14:15:34 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*ptr;

	if (!lst || !new)
		return ;
	if (*lst == NULL)
		*lst = new;
	else
	{
		ptr = ft_lstlast(*lst);
		ptr->next = new;
	}
}

/*
#include <stdio.h>
void    printlist(t_list *head)
{
    while (head != NULL)
    {
        printf("%s\n", (char *)head->content);
        head = head->next;
    }
}

int main(void)
{
    t_list s1;
    t_list s2;
    t_list s3;
	t_list *lst;
    t_list *newlst;
	t_list new;
	
	lst = &s1;
    newlst = &new;
    s1.content = "abc";
    s1.next = &s2;
    s2.content = "def";
    s2.next = NULL;
    new.content = "Hello";
    new.next = &s3;
    s3.content = "World";
    s3.next = NULL;
    printlist(lst);
    ft_lstadd_back(&lst, newlst);
    printf("--------------\n");
    printlist(lst);
}
*/