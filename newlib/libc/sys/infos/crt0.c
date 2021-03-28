#include "syscall.h"
 
extern void exit(int code);
extern int main(int argc, const char *argv[]);
 
void _start(const char *cmdline)
{
    __console_handle = open("/dev/console", 0, 0);
    if (is_error(__console_handle)) exit(1);
    
    const char* argv[] = {cmdline, 0};
    int _exit_code = main(0, argv);
    close(__console_handle);
    exit(_exit_code);
}
