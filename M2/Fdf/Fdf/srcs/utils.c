/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/26 11:18:33 by vagarcia          #+#    #+#             */
/*   Updated: 2025/01/27 16:02:28 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/fdf.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	while (s1[i] || s2[i])
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}

int	ft_atoi(const char *str)
{
	unsigned long	number;
	int				i;
	int				sign;

	i = 0;
	sign = 1;
	if (!str)
		return (0);
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			sign = -1;
	number = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		number = number * 10 + str[i++] - '0';
		if (sign == 1 && number >= L_MAX)
			return (-1);
		else if (sign == -1 && number >= (unsigned long)L_MAX + 1)
			return (0);
	}
	return ((int)number * sign);
}

void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	i = 0;
	if (!s)
		return ;
	while (s[i])
	{
		write(fd, &s[i], 1);
		i++;
	}
}

int	get_color(void)
{
	static int	colors[255] = {C0, C1, C2, C3, C4, C250, C6, C7, C8, C238,
		C9, C241, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22,
		C23, C24, C25, C26, C27, C28, C29, C30, C31, C32, C33, C34, C35, C36,
		C37, C38, C39, C40, C41, C42, C43, C44, C45, C46, C47, C48, C49, C50,
		C51, C52, C53, C54, C55, C56, C57, C58, C59, C60, C61, C62, C63, C64,
		C65, C66, C67, C68, C69, C70, C71, C72, C73, C74, C75, C76, C77, C78,
		C79, C80, C81, C82, C83, C84, C85, C86, C87, C88, C89, C90, C91, C92,
		C93, C94, C95, C96, C97, C98, C99, C100, C101, C102, C103, C104, C105,
		C114, C107, C108, C109, C110, C111, C112, C113, C114, C115, C116, C117,
		C118, C119, C120, C121, C122, C123, C124, C125, C126, C127, C128, C129,
		C130, C131, C132, C133, C134, C135, C136, C137, C138, C139, C140, C141,
		C142, C143, C242, C145, C146, C147, C148, C149, C150, C151, C152, C153,
		C154, C155, C156, C157, C158, C159, C160, C243, C162, C163, C164, C165,
		C166, C167, C168, C169, C170, C171, C172, C173, C174, C175, C176, C177,
		C178, C179, C180, C181, C182, C183, C184, C185, C186, C187, C188, C189,
		C190, C191, C192, C193, C194, C195, C196, C197, C198, C199, C200, C201,
		C202, C203, C204, C205, C206, C207, C208, C209, C210, C211, C212, C213,
		C214, C215, C216, C217, C218, C219, C220, C221, C222, C223, C224, C225,
		C226, C227, C228, C229, C230, C231, C232, C233, C234, C235, C236, C237};
	static int	color_index = 0;
	int			color;

	color = colors[color_index];
	color_index = (color_index + 1) % (255 / sizeof(colors[0]));
	return (color);
}

int	count_rows(int fd, char *line)
{
	int		i;
	char	*temp_line;

	i = 0;
	if (!line)
		return (0);
	temp_line = line;
	while (temp_line)
	{
		i++;
		free(temp_line);
		temp_line = get_next_line(fd);
	}
	if (temp_line)
	{
		free(temp_line);
		i++;
	}
	return (close(fd), i);
}
