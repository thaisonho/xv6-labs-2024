#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
readline (char *new_argv[32], int curr_argc)
{
  char buf[1024];
  int n = 0;
  //stdin: abcd\n
  //n: 4
  //buffer: abc
  while (read (0, buf + n, 1))  // 0 la stdin, 1 stdout 
    {
      if (n == 1023)
        {
          fprintf (2, "argument is too long\n");
          exit (1);
        }
      if (buf[n] == '\n')
        {
          break;
        }
      n++;
    }
  buf[n] = 0;
  if (n == 0)
    return 0;

  //new_argv: echo, hellp
  //buffer: hello world 12

  int offset = 0;
  while (offset < n)
    {
      int arg_len = 0;
      while (buf[offset + arg_len] != ' ' && buf[offset + arg_len] != 0)
        arg_len++;

      new_argv[curr_argc] = malloc (arg_len + 1);
      memcpy (new_argv[curr_argc], buf + offset, arg_len);
      new_argv[curr_argc][arg_len] = 0;
      curr_argc++;
      offset += arg_len;
      while (buf[offset] == ' ')
        offset++;
    }

  //new_argv co : echo, hellp, hello, world, 12

  return curr_argc; //5
}

// echo hello | xargs echo "world" 
// world hello 
// dau tien tao process 1 de chay echo hello. stdout luc nay co hello
//  | chuyen stdout process 1 thanh stdin cho process 2, process 2 la chay xargs echo "world"
// xargs bien tung tu trong stdin thanh parameter vaf them cho echo 

int
main (int argc, char const *argv[])
{
  if (argc <= 1)
    {
      fprintf (2, "Usage: xargs command (arg ...)\n");
      exit (1);
    }
  char *command = malloc (strlen (argv[1]) + 1);
  char *new_argv[MAXARG];
  strcpy (command, argv[1]);
  for (int i = 1; i < argc; ++i)
    {
      new_argv[i - 1] = malloc (strlen (argv[i]) + 1);
      strcpy (new_argv[i - 1], argv[i]);
    }
    // new_args gom:  echo, "world"
    // command la echo 
    // argv gom:  xarg, echo, "world", 0
    // argc la 3 

    //stdin: helo worl
  int curr_argc;
  while ((curr_argc = readline (new_argv, argc - 1)) != 0)
    {
      new_argv[curr_argc] = 0;
      int pid = fork ();

      //process cha dang thuc hien main xargs hien tai
      // exec (command, new_argv); tai process cha
      // thay vi van chuong trinh xargs hien tai, no se goi comand va khong thuc hien
      // chuong trinh main hien tai

      // tao process con de thuc hien command moi 
      if ( pid == 0)
        {
          //process con
          //command = xargs
          exec (command, new_argv); //return lai neu failed khi thu hien command voi argument

          //neu failed
          fprintf (2, "Exec failed\n");
          exit (1);
        }
      else{
          //process cha
          wait (0);
      }
      
    }
  exit (0);
}
