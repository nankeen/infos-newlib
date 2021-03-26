#pragma once
#include <sys/_types/_ssize_t.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/errno.h>

#define SYS_NOP             0x00
#define SYS_YIELD           0x01
#define SYS_EXIT            0x02
#define SYS_OPEN            0x03
#define SYS_CLOSE           0x04
#define SYS_READ            0x05
#define SYS_WRITE           0x06
#define SYS_OPENDIR         0x07
#define SYS_READDIR         0x08
#define SYS_CLOSEDIR        0x09
#define SYS_EXEC            0x0a
#define SYS_WAIT_PROC       0x0b
#define SYS_CREATE_THREAD   0x0c
#define SYS_STOP_THREAD     0x0d
#define SYS_JOIN_THREAD     0x0e
#define SYS_USLEEP          0x0f
#define SYS_GET_TOD         0x10
#define SYS_MMAP            0x15

typedef unsigned long HANDLE;
typedef HANDLE HFILE;
typedef HANDLE HDIR;
typedef HANDLE HPROC;
typedef HANDLE HTHREAD;

struct tod {
    unsigned short seconds, minutes, hours, day_of_month, month, year;
};

extern char **environ;
void _exit(unsigned int exit_code);
int close(HFILE file);
HPROC execve(char *name, char **argv, char **env);
int fork();
int fstat(int file, struct stat *st);
int getpid();
int isatty(int file);
int kill(int pid, int sig);
int link(char *old, char *new_link);
int lseek(int file, int ptr, int dir);
HFILE open(const char *name, int flags, int mode); 
ssize_t read(HFILE file, char *ptr, size_t len);
caddr_t sbrk(int incr);
int stat(const char *file, struct stat *st);
clock_t times(struct tms *buf);
int unlink(char *name);
int wait(int *status);
ssize_t write(HFILE file, const char *ptr, size_t len);
int gettimeofday(struct timeval *p, struct timezone *z);
