#include "syscall.h"
 
extern void exit(int code);
extern int main();
HFILE __console_handle;
 
void _start()
{
    __console_handle = open("/dev/console", 0, 0);
    if (is_error(__console_handle)) exit(1);
    
    int _exit_code = main();
    close(__console_handle);
    exit(_exit_code);
}
