/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:19:15 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 20:48:33 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*ptr;

	if (size != 0 && (nmemb * size) / size != nmemb)
		return (NULL);
	ptr = malloc(nmemb * size);
	if (!ptr)
		return (NULL);
	if (nmemb == 0 || size == 0)
		return (ptr);
	ft_bzero(ptr, nmemb * size);
	return (ptr);
}

/*
#include <stdio.h>
int main(int argc, char **argv)
{
	void *ptr1;
	void *ptr2;

	(void)argc;
	ptr1 = calloc(atoi(argv[1]), atoi(argv[2]));
	printf("calloc:\n");
	if (ptr1 == (void*)0)
		printf("bad\n");
	else
		printf("good\n");
	free(ptr1);
	ptr2 = ft_calloc(atoi(argv[1]), atoi(argv[2]));
	    printf("ft_calloc:\n");
    if (ptr2 == (void*)0)
        printf("bad\n");
    else
		printf("good\n");
	free(ptr2);
	return (0);
}
*/
