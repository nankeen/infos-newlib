#include "syscall.h"
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>

uint64_t syscall(uint32_t nr, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4, uint64_t a5)
{
    uint64_t ret;

    register uint64_t r8 asm("r8") = a5;
    
    asm volatile("int $0x81" : "=a"(ret) : "a"((uint64_t)nr), "D"(a1), "S"(a2), "d"(a3), "c"(a4), "r"(r8) : "r11");
    
    return ret;
}
 
void _exit()
{

    asm volatile("int $0x81" : : "a"((uint64_t)SYS_EXIT), "D"(a1), "S"(a2), "d"(a3), "c"(a4), "r"(r8) : "r11");
}
