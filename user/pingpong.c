#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/*need to use these below bc linter always show errors*/
/*only use these if your linter shows error (like clangd :D) */
/*this works just fine (tested with ./grade-lab-until and make qemu)*/
/*#include "../kernel/types.h"*/
/*#include "../kernel/stat.h"*/
/*#include "user.h"*/

int
main (int argc, char **argv)
{
  // False argument handle
  if (argc > 1)
    {
      printf ("Usage: pingpong (no extra arguments needed)\n");
      exit (1);
    }

  char buf[1]; // buffer for sending bytes

  int fd1[2]; // for parent - child
  int fd2[2]; // for child - parent
  pipe (fd1);
  pipe (fd2);

  int pid = fork ();

  if (pid > 0)
    { // parent
      // Close unused ends of file descriptors
      close (fd1[0]);
      close (fd2[1]);

      write (fd1[1], "a", 1); // send byte to child

      if (read (fd2[0], buf, 1) != -1) // receive byte from child
        printf ("%d: received pong\n", getpid ());
      else
        printf ("Child sent byte unsucessful");

      // finish using these, so just close them
      close (fd1[1]);
      close (fd2[0]);

      wait (0); // wait until child finished
      exit (0);
    }
  else if (pid == 0)
    {
      // Close unused ends of file descriptors
      close (fd1[1]);
      close (fd2[0]);

      if (read (fd1[0], buf, 1) != -1) // read byte from parent
        printf ("%d: received ping\n", getpid ());
      else
        printf ("Parent sent byte unsucessful\n");
      write (fd2[1], buf, 1); // send bytes to parent

      // finish using these, so just close them
      close (fd1[0]);
      close (fd2[1]);
      exit (0);
    }
  else
    {
      printf ("Create child process failed\n");
      exit (1);
    }
}
