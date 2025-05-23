/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 11:53:33 by vagarcia          #+#    #+#             */
/*   Updated: 2024/10/14 11:53:35 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10000
# endif // BUFFER_SIZE

char	*get_next_line(int fd);
char	*readfile(int fd, char *res);
char	*nextline(char	*buf);
char	*makeline(char	*buf);
void	*bytecheck(ssize_t bytes, char *buf, char *line);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_free(char *buffer, char *new);
char	*ft_strchr(const char *str, int car);
size_t	ft_strlen(const char *str);
void	*ft_calloc(size_t nmemb, size_t size);

#endif // GET_NEXT_LINE_BONUS_H
