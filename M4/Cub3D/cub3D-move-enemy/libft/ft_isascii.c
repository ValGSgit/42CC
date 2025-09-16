/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 16:27:16 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 18:37:06 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isascii(int c)
{
	if ((c >= 0 && c <= 127))
		return (1);
	else
		return (0);
}

/*
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
int	main(int argc, char **argv)
{
	(void)argc;
	printf("%d\n", ft_isascii(atoi(argv[1])));
	printf("%d\n", isascii(atoi(argv[1])));
	return (0);
}
*/
