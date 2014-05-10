// $Id: xt.c,v 1.10 2001/02/09 06:46:09 querbach Exp $

// xt.c				   Copyright (C) 2001, Real-Time Systems Inc.
//------------------------------------------ www.realtime.bc.ca -------------
//
//	Simple Terminal Emulator
//
//	Derived from miniterm.c (by Sven Goldt, goldt@math.tu.berlin.de).
//
//	This program is free software; you can redistribute it and/or
//	modify it under the terms of the GNU General Public License
//	as published by the Free Software Foundation; either version 2
//	of the License, or (at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//---------------------------------------------------------------------------

#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>


// global variables

char* argv0 = "xt";		// program name

char* device = "";		// device name
int fd = 0;			// file descriptor

long baud = 9600;		// baud rate as integer
speed_t speed = B9600;		// terminal speed constant
int stopbit = 1;		// stop bit, can be 1 or 2

volatile int stopReq = 0; 	// stop request from child process

struct termios oldStdinTio;	// old console settings
struct termios newStdinTio;	// new console settings

struct termios oldSerialTio;	// old serial port settings
struct termios newSerialTio;	// new serial port settings


// clean up, exit

void quit(int code)
{
  if (fd > 0)
    tcsetattr(0, TCSANOW, &oldSerialTio);
  tcsetattr(0, TCSANOW, &oldStdinTio);
  printf("\n");
  exit(code);
}


// show copyright message, exit

void copyright(void)
{
  printf("%s 1\n\n", argv0);
  printf("Copyright (C) 2001 Real-Time Systems, Inc. (www.realtime.bc.ca)\n");
  printf("This is free software, see the source for copying conditions.");
  printf("  There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS");
  printf(" FOR A PARTICULAR PURPOSE.\n");
  quit(0);
}


// show usage, exit

void usage(void)
{
  printf("Usage: %s -p port [-b baudrate] [-s stopbits] [--version]", argv0);

  printf("xkn add his step here\n");
  quit(1);
}


// show error message, exit

void error(const char* msg)
{
  if (errno)
    printf("%s: %s: %s", argv0, strerror(errno), msg);
  else
    printf("%s: %s", argv0, msg);
  quit(1);
}


// handle SIGCHLD from child process

void child_handler
(
  int dummy
)
{
  stopReq = 1;
}


// program mainline

int main
(
  int argc,			// argument count
  char* argv[]			// argument vector
)
{
  int done;
  int i = 1;

  argv0 = argv[0];
  
  // capture current terminal settings
  
  tcgetattr(0, &oldStdinTio);

  // check argument count
  printf("hellow from xknxknqq  it is a develop requst with a test fix \n"); 

  if (argc < 2)
    usage();

  // parse command line


  
  for (i; i < argc; i++)
    if (!strcmp(argv[i], "--version"))
      copyright();

  printf("xknxkn add his own step after the copyright\n");

  done = 0;
  while (!done)
  {
    switch (getopt(argc, argv, "p:b:s:"))
    {
      case 'p':				// set device
        device = optarg;
        break;
        
      case 'b':				// set baud rate
        baud = strtol(optarg, 0, 10);
        switch (baud)
	{
          case      0:  speed =      B0;  break;
          case     50:  speed =     B50;  break;
          case     75:  speed =     B75;  break;
          case    110:  speed =    B110;  break;
          case    134:  speed =    B134;  break;
          case    150:  speed =    B150;  break;
          case    200:  speed =    B200;  break;
          case    300:  speed =    B300;  break;
          case    600:  speed =    B600;  break;
          case   1200:  speed =   B1200;  break;
          case   1800:  speed =   B1800;  break;
          case   2400:  speed =   B2400;  break;
          case   4800:  speed =   B4800;  break;
          case   9600:  speed =   B9600;  break;
          case  19200:  speed =  B19200;  break;
          case  38400:  speed =  B38400;  break;
          case  57600:  speed =  B57600;  break;
          case 115200:  speed = B115200;  break;
//        case 230400:  speed = B230400;  break;

          default:
            error("unknown baud rate");
        }	
        break;

      case 's':
        stopbit = atoi(optarg);
	if ((stopbit != 1) && (stopbit != 2))
	  error("unknown number of stop bits");
        break;
                
      case '?':				// unknown argument
        usage();
        
      case -1:				// no more arguments
        done = 1;
        break;
    }        
  }

  if (!device)
    usage();
  
  // set console into raw mode
    
  tcgetattr(0, &newStdinTio);
  newStdinTio.c_lflag = 0;
  tcsetattr(0, TCSAFLUSH, &newStdinTio);

  // open device, set to transparent mode
  
  if ((fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0)
    error(device); 

  tcgetattr(fd, &oldSerialTio);
  tcgetattr(fd, &newSerialTio);
  newSerialTio.c_cflag = CS8 | CLOCAL | CREAD;
  if (stopbit == 2)
    newSerialTio.c_cflag |= CSTOPB;
  newSerialTio.c_iflag = IGNPAR;
  newSerialTio.c_oflag = 0;
  newSerialTio.c_lflag = 0;
  cfsetspeed(&newSerialTio, speed);
  tcsetattr(fd, TCSAFLUSH, &newSerialTio);
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) & ~O_NONBLOCK);
  
  switch (fork())
  {
    // error in fork()

    case -1:
    {
      error("fork");
    }

    // child process -- copies standard input to serial port

    case 0:
    {
      // copy standard input to serial port

      int c = '\r';
      write(fd, &c, 1);

      while (1)
      {
        int c;

        read(0, &c, 1);

        if (c < 0)		// end of file? exit
          break;

        c &= 0xFF;

        if (c == 0x1B)		// escape character?  exit
          break;

        else if (c == 0x02)	// ^B? send line break
          tcsendbreak(fd, 0);
          
        else if (c == 0x7F)	// map del to backspace
          c = 'H' - 0x40;

        else if (c == '\n')	// map newline to carriage return
          c = '\r';

        write(fd, &c, 1);
      }

      // shut down after sending SIGCHLD to parent

      exit(0);
    }

    // parent process -- copies serial port to standard output

    default:
    {
      // prepare handler for child's termination

      struct sigaction sa;

      sa.sa_handler = child_handler;
      sa.sa_flags = 0;
      sigaction(SIGCHLD, &sa, NULL);

      // copy serial port to standard ouput

      while (1)
      {
        int c;

        read(fd, &c, 1);
        if (stopReq)
          break;

        write(1, &c, 1);
        if (stopReq)
          break;
      }

      // wait for child to die, then exit with success

      wait(NULL);
      quit(0);
    }
  }
}

