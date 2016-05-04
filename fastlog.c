/**************************************************
 * U1282A fastlog
 *
 * rev 1 - shabaz - May 2016
 *
 *
 * Example usage:
 * fastlog                  - dumps 10 results to the display
 * fastlog 100              - dumps 100 results to the display
 * fastlog -t /dev/ttyUSB0  - use /dev/ttyUSB0 (this is the default)
 * fastlog -o log.csv       - write to CSV file 
 *
 **************************************************/


#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

//#define DO_DEBUG

#define DEFAULT_TTY "/dev/ttyUSB0"
#define DEFAULT_FNAME "out.csv"

int ttyfd;

int
delay_ms(unsigned int msec)
{
  int ret;
  struct timespec a;
  if (msec>999)
  {
    fprintf(stderr, "delay_ms error: delay value needs to be less than 999\n");
    msec=999;
  }
  a.tv_nsec=((long)(msec))*1E6d;
  a.tv_sec=0;
  if ((ret = nanosleep(&a, NULL)) != 0)
  {
    fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
  }
  return(0);
}

int
do_transact(char* rx, char* tx, char* tstring)
{
  int i, j;
  char cbuf;
  char* bufptr;
  char txbuf[128];
  char timebuf[128];
  bufptr=rx;
  int rb;
  struct timespec ts;
  long sec;
  int msec;
  time_t cur_time;
  struct tm* stm;

  strcpy(txbuf, tx);
  i=strlen(txbuf);
#ifdef DO_DEBUG
  printf("string is %s length %d\n", txbuf, i);
#endif
  txbuf[i]='\n';

  write(ttyfd, txbuf, i+1);

  i=0;
  do
  {
    rb = read(ttyfd, &cbuf, 1);
    rx[i]=cbuf;
    if (i==0)
    {
      clock_gettime(CLOCK_REALTIME, &ts);
      //cur_time=time(NULL);
    }
    i++;
#ifdef DO_DEBUG
    if (cbuf>31)
      printf("%c", cbuf);
    else
      printf("[%d]", cbuf);
    fflush(stdout);
#endif
  } while ((cbuf != '\n') && (rb > 0));

  sec=ts.tv_sec;
  msec=(int)(ts.tv_nsec/1000000);
  cur_time=sec;
  stm=localtime(&cur_time);
  strftime(timebuf, sizeof(timebuf), "%m/%d/%Y %H:%M:%S", stm);
  if (tstring!=NULL)
    sprintf(tstring, "%s.%03d", timebuf, msec);

#ifdef DO_DEBUG
  printf("\n");
  printf("\nnumber of bytes read is %d\n", i);
#endif

  rx[i]='\0';
  for (j=i-1; j>=0; j--)
  {
    if (rx[j]<31)
    {
      rx[j]='\0';
    }
    else
    {
      break;
    }
  }

  return(0);
}

int
main(int argc, char *argv[])
{
  char ttyname[64];
  char ofname[256];
  unsigned long iter, k;
  int i;
  char cbuf;
  int dofile=0;
  FILE* ofd;
  
  char tbuf[256];
  char buf[1024];
  struct termios options;

  char param[128];

  strcpy(ttyname, DEFAULT_TTY);
  iter=10;
  strcpy(ofname, DEFAULT_FNAME);

  if (argc>1)
  {
    for (i=1; i<argc; i++)
    {
      strcpy(param, argv[i]);
      if (strncmp("-t", param, 2)==0)
      {
        if (i+1<argc)
        {
          strcpy(ttyname, argv[i+1]);
          i++;
        }
      }
      else if (strncmp("-o", param, 2)==0)
      {
        if (i+1<argc)
        {
          strcpy(ofname, argv[i+1]);
          dofile=1;
          i++;
        }
      }
      else if (isdigit(param[0]))
      {
        sscanf(param, "%lu", &iter);
      }

    }
  }
    
#ifdef DO_DEBUG
  printf("Using port %s\n", ttyname);
  printf("Output file status %d\n", dofile);
  printf("File name %s\n", ofname);
  printf("Iter %lu\n", iter);
#endif
  
  ttyfd = open(ttyname, O_RDWR | O_NOCTTY | O_NDELAY);
  if (ttyfd == -1)
  {
    perror("open_port: Unable to open:");
    return(-1);
  }
  else
  {
    fcntl(ttyfd, F_SETFL, 0);
  }

  if (dofile)
  {
    ofd=fopen(ofname, "w");
    if (ofd==NULL)
    {
      perror("file open error");
      return(-1);
    }
  }

  tcgetattr(ttyfd, &options);

  cfsetispeed(&options, B9600);
  cfsetospeed(&options, B9600);
  options.c_cflag |= (CLOCAL | CREAD);
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
  options.c_cflag &= ~CRTSCTS;
  options.c_cc[VMIN] = 1;
  options.c_cc[VTIME] = 5;
  cfmakeraw(&options);

  tcflush(ttyfd, TCIFLUSH);
  tcsetattr(ttyfd, TCSANOW, &options);


  do_transact(buf, "*IDN?", NULL);
  printf("%s\n", buf);
  if (dofile) fprintf(ofd, "%s\n", buf);
  do_transact(buf, "CONF?", NULL);
  printf("Configuration: %s\n", buf);
  if (dofile) fprintf(ofd, "Configuration: %s\n", buf);
  do_transact(buf, "SYST:BATT?", NULL);
  printf("Battery: %s\n", buf);
  if (dofile) fprintf(ofd, "Battery: %s\n", buf);
  for (k=0; k<iter; k++)
  {
    do_transact(buf, "FETC?", tbuf);
    printf("%lu, %s, %s\n", k+1, tbuf, buf);
    if (dofile)
    {
      fprintf(ofd, "%lu, %s, %s\n", k+1, tbuf, buf);
    }
    delay_ms(100);
  }

  close(ttyfd);
  if (dofile)
  {
    fclose(ofd);
  }
  return (0);
}

