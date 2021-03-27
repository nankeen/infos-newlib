#include "syscall.h"

#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

HFILE __console_handle;

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
    // Hack to support the different conventions between InfOS and POSIX
    if (file == STDIN_FILENO || file == STDOUT_FILENO || file == STDERR_FILENO) {
        file = __console_handle;
    }
    ssize_t ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_READ), "D"(file), "S"(ptr), "d"(len));
    return ret;
}


static void *mmap(void *addr, size_t len, int flags, HFILE fd, off_t offset)
{
    void *ret;
    
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_MMAP), "D"(addr), "S"(len), "d"(flags), "c"(fd), "r"(offset));
    
    return ret;
}

caddr_t sbrk(int incr)
{
    // We implement this with mmap instead
    extern char _end;		/* Defined by the linker */
    static void *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = (void *)(__align_up_page((uintptr_t)&_end) + (__page_size << 2));
        mmap(heap_end, 0x5000, -1, 0, 0);
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

int unlink(char *name)
{
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
    // Hack to support the different conventions between InfOS and POSIX
    if (file == STDIN_FILENO || file == STDOUT_FILENO || file == STDERR_FILENO) {
        file = __console_handle;
    }
    ssize_t ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_WRITE), "D"(file), "S"(ptr), "d"(len));
    return ret;
}
