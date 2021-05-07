#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void)
{
  char user_id[MAXLINE];
  char password[MAXLINE];

  /* The user will type in a user name on one line followed by a password 
     on the next.
     DO NOT add any prompts.  The only output of this program will be one 
	 of the messages defined above.
     Please read the comments in validate carefully.
   */

  if (fgets(user_id, MAXLINE, stdin) == NULL)
  {
    perror("fgets");
    exit(1);
  }
  if (fgets(password, MAXLINE, stdin) == NULL)
  {
    perror("fgets");
    exit(1);
  }

  //pipe
  int fd[2];
  if (pipe(fd) == -1)
  {
    perror("pipe");
    exit(1);
  }

  int r = fork();

  if (r > 0)
  {
    if (close(fd[0]) == -1)
    {
      perror("close");
      exit(1);
    }
    if (write(fd[1], user_id, 10) == -1)
    {
      perror("write");
      exit(1);
    }
    if (write(fd[1], password, 10) == -1)
    {
      perror("write");
      exit(1);
    }
    if (close(fd[1]) == -1)
    { // close the write pipe
      perror("close");
      exit(1);
    }
  }
  else if (r < 0)
  {
    perror("fork");
    exit(1);
  }
  else
  {
    if (dup2(fd[0], STDIN_FILENO) == -1)
    {
      perror("dup2");
      exit(1);
    }
    // close file descriptors
    if (close(fd[0]) == -1)
    {
      perror("close");
      exit(1);
    }
    if (close(fd[1]) == -1)
    {
      perror("close");
      exit(1);
    }
    // execute validate
    execl("./validate", "validate", NULL);
    perror("execl");
    exit(1);
  }

  int status;
  wait(&status);
  if (WIFEXITED(status))
  {
    if (WEXITSTATUS(status) == 0)
    {
      printf(SUCCESS);
    }
    else if (WEXITSTATUS(status) == 1)
    {
      exit(1);
    }
    else if (WEXITSTATUS(status) == 2)
    {
      printf(INVALID);
    }
    else if (WEXITSTATUS(status) == 3)
    {
      printf(NO_USER);
    }
  }

  return 0;
}
