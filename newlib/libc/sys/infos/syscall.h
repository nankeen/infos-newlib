#pragma once
#include <sys/types.h>
#include <sys/times.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <stddef.h>

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
#define SYS_GET_TICKS       0x12
#define SYS_PREAD           0x13
#define SYS_PWRITE          0x14
#define SYS_MMAP            0x15
#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2

#define __page_bits 12lu
#define __page_size (1lu << __page_bits)

#define __align_up(__value, __alignment) (__value + ((__value % __alignment == 0lu) ? 0lu : (__alignment - (__value % __alignment))))
#define __align_up_page(__value) __align_up(__value, __page_size)

typedef unsigned long HANDLE;
typedef HANDLE HFILE;
typedef HANDLE HDIR;
typedef HANDLE HPROC;
typedef HANDLE HTHREAD;

struct tod {
    unsigned short seconds, minutes, hours, day_of_month, month, year;
};

static inline int is_error(long h) { return h == -1; }

extern char **environ;
extern HFILE __console_handle;
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
caddr_t sbrk(ptrdiff_t size);
int stat(const char *file, struct stat *st);
clock_t times(struct tms *buf);
int unlink(char *name);
int wait(int *status);
ssize_t write(HFILE file, const char *ptr, size_t len);
int usleep(useconds_t useconds);
ssize_t pread(int fd, void *buf, size_t count, off_t offset);
clock_t clock();
