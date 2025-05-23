/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 16:12:56 by kfan              #+#    #+#             */
/*   Updated: 2024/09/19 14:12:35 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_front(t_list **lst, t_list *new)
{
	if (!new || !lst)
		return ;
	new->next = *lst;
	*lst = new;
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
    ft_lstadd_front(&lst, newlst);
    printf("--------------\n");
    printlist(lst);
}
*/