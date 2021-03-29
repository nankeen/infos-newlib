#include "syscall.h"

#undef errno
extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

HFILE __console_handle;

void _exit(unsigned int exit_code)
{
    asm volatile("int $0x81" : : "a"((uint64_t)SYS_EXIT), "D"(exit_code));
    __builtin_unreachable();
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

static int sbrk_needs_init = 1;
static caddr_t sbrk_curbrk;
static size_t sbrk_region_size = 20*1024*1024;

caddr_t sbrk(int size)
{
    // We implement this with mmap instead
    // _end is defined by the linker
    char *prev_heap_end;

    if (sbrk_needs_init) {
        sbrk_needs_init = 0;
        // Simulate with mmap
        sbrk_curbrk = (caddr_t)(__align_up_page(0x10000000) + (__page_size << 2));
        if (mmap(sbrk_curbrk, sbrk_region_size, -1, 0, 0) < 0) {
            errno = ENOMEM;
            return (caddr_t)-1;
        }
    }
    
    if (size <= 0) {
        return sbrk_curbrk;
    } else if (size > sbrk_region_size) {
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    sbrk_curbrk += size;
    sbrk_region_size -= size;
    return (sbrk_curbrk - size);
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

int usleep(useconds_t useconds)
{
    int ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_USLEEP), "D"(useconds));
    return ret;
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset)
{
    ssize_t ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_PREAD), "D"(fd), "S"(buf), "d"(count), "c"(offset));
    return ret;
}

clock_t clock()
{
    ssize_t ret;
    asm volatile("int $0x81" : "=a"(ret) : "a"(SYS_GET_TICKS));
    return ret;
}
