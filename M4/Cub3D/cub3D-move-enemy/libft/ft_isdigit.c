/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfan <kfan@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 16:27:16 by kfan              #+#    #+#             */
/*   Updated: 2024/09/20 18:37:14 by kfan             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isdigit(int c)
{
	if ((c >= 48 && c <= 57))
		return (2048);
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
	printf("%d\n", ft_isdigit(atoi(argv[1])));
	printf("%d\n", isdigit(atoi(argv[1])));
	return (0);
}
*/
