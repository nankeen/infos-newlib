#include <fcntl.h>
 
extern void exit(int code);
extern int main();
 
void _start()
{
    int _exit_code = main();
    exit(_exit_code);
}

