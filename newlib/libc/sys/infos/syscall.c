#include "syscall.h"

#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

static unsigned long syscall(uint32_t nr, unsigned long a1, unsigned long a2, unsigned long a3, unsigned long a4, unsigned long a5)
{
    unsigned long ret;

    register unsigned long r8 asm("r8") = a5;
    
    asm volatile("int $0x81" : "=a"(ret) : "a"((unsigned long)nr), "D"(a1), "S"(a2), "d"(a3), "c"(a4), "r"(r8) : "r11");
    
    return ret;
}
 
void _exit(unsigned int exit_code)
{
    asm volatile("int $0x81" : : "a"((uint64_t)SYS_EXIT), "D"(exit_code));
}

int close(HANDLE handle)
{
    int ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"((uint64_t)SYS_CLOSE), "D"(handle));
    return ret;
}

HPROC execve(char *name, char **argv, char ** env)
{
    HPROC ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_EXEC), "D"(name), "S"(argv[0]));
    return ret;
}

int fork(void)
{
    errno = EAGAIN;
    return -1;
}

int fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int getpid(void)
{
    return 1;
}

int isatty(int file)
{
    return 1;
}

int kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}

int link(char *old, char *new)
{
    errno = EMLINK;
    return -1;
}

int lseek(int file, int ptr, int dir)
{
    return 0;
}

HFILE open(const char *name, int flags, int mode)
{
    HFILE ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_OPEN), "D"(name), "S"(flags));
    return ret;
}

ssize_t read(HFILE file, char *ptr, size_t len)
{
    ssize_t ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_READ), "D"(file), "S"(ptr), "d"(len));
    return ret;
}

// TODO: Implement this with mmap
caddr_t sbrk(int incr) {
    extern char _end;		/* Defined by the linker */
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

int stat(const char *file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

clock_t times(struct tms *buf)
{
    return -1;
}

int unlink(char *name){
    errno = ENOENT;
    return -1; 
}

int wait(int *status)
{
    errno = ECHILD;
    return -1;
}


ssize_t write(HFILE file, const char *ptr, size_t len)
{
    ssize_t ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_WRITE), "D"(file), "S"(ptr), "d"(len));
    return ret;
}

static int is_leap_year(time_t year)
{
    return (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0));
}

static time_t month_to_days(struct tod *time_of_day)
{
    time_t days = 0;
    if (time_of_day->month > 1) {
        days += 31;
    }
    if (time_of_day->month > 2) {
        days += is_leap_year(time_of_day->year) ? 29 : 28;
    }
    if (time_of_day->month > 3) {
        days += 31;
    }
    if (time_of_day->month > 4) {
        days += 30;
    }
    if (time_of_day->month > 5) {
        days += 31;
    }
    if (time_of_day->month > 6) {
        days += 30;
    }
    if (time_of_day->month > 7) {
        days += 31;
    }
    if (time_of_day->month > 8) {
        days += 31;
    }
    if (time_of_day->month > 9) {
        days += 30;
    }
    if (time_of_day->month > 10) {
        days += 31;
    }
    if (time_of_day->month > 11) {
        days += 30;
    }
    return days;
}

static time_t year_to_days(struct tod *time_of_day)
{
    time_t days = 365 * (time_of_day->year - ((time_of_day->year < 100) ? 70 : 1970));
    // Account for leap years
    days += (time_of_day->year - ((time_of_day->year < 100) ? 68 : 1968)) / 4;
    return days;
}

int gettimeofday(struct timeval *p, struct timezone *z)
{
    struct tod time_of_day;
    time_t secs_in_day = 86400;
    asm volatile("int $0x81" : : "a"(SYS_GET_TOD), "D"(&time_of_day));

    p->tv_sec = time_of_day.seconds;
    p->tv_sec += time_of_day.minutes * 60;
    p->tv_sec += time_of_day.hours * 60 * 60;

    time_t days_since_epoch = time_of_day.day_of_month;
    days_since_epoch += month_to_days(&time_of_day);
    days_since_epoch += year_to_days(&time_of_day);
    p->tv_sec += days_since_epoch * secs_in_day;
    p->tv_usec = 0;

    z->tz_dsttime = 0;
    z->tz_minuteswest = 0;

    return 0;
}
