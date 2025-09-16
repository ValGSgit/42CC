/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   playtestfuncs.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vagarcia <vagarcia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 11:21:45 by vagarcia          #+#    #+#             */
/*   Updated: 2024/11/16 14:14:53 by vagarcia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// open, close, read, write,
// malloc, free, perror,
// strerror, access, dup, dup2,
// execve, exit, fork, pipe,
// unlink, wait, waitpid

// Known: open, close, read, write, malloc, free, exit;

// Sort-of-know: pipe, wait, perror

// Unknown: fork, waitpid, unlink, execve, access, dup & dup2

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "pipex.h"

int main()
{

    // make two process which run same
    // program after this instruction
    pid_t p = fork();
    if(p<0){
      perror("fork fail");
      exit(1);
    }
    printf("Hello world!, process_id(pid) = %d \n",getpid());
    return 0;
}

void forkexample()
{
    pid_t p;
    p = fork();
    if(p<0)
    {
      perror("fork fail");
      exit(1);
    }
    // child process because return value zero
    else if ( p == 0)
        printf("Hello from Child!\n");

    // parent process because return value non-zero.
    else
        printf("Hello from Parent!\n");
}
int main()
{
    forkexample();
    return 0;
}

void forkexample()
{
    int x = 1;
    pid_t p = fork();
      if(p<0){
      perror("fork fail");
      exit(1);
    }
    else if (p == 0)
        printf("Child has x = %d\n", ++x);
    else 
        printf("Parent has x = %d\n", --x);
}
int main()
{
    forkexample();
    return 0;
}