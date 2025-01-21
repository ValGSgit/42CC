/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 13:12:46 by vagarcia          #+#    #+#             */
/*   Updated: 2024/12/02 11:06:19 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <stdlib.h>
# include <limits.h>
# include <sys/types.h>
# include <sys/uio.h>
# include <unistd.h>
# include <fcntl.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10
# endif // BUFFER_SIZE

# ifndef MAX_FD
#  define MAX_FD 1024
# endif // MAX_FD

char	*get_next_line(int fd);
char	*readfile(int fd, char *res);
char	*nextline(char	*buf);
char	*makeline(char	*buf);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_freejoin(char *buffer, char *new);
char	*ft_strchr(const char *str, int car);
size_t	ft_strlen(const char *str);
void	*ft_calloc(size_t nmemb, size_t size);

#endif // GET_NEXT_LINE_BONUS_H
