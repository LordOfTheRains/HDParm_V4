#include <types.h>
#include <file.h>
//#include <syscall.h>
#include <thread.h>
#include <curthread.h>

/*sample implementation of sys_getpid() */

//int sys_open(const char *path, int oflag, mode_t mode)
int
sys_open(const char *path, int oflag, int mode)
{
    kprintf("inside open syscall\n");
    return 0;
}


int
sys_read(int fd, void *buf, size_t nbytes)
{
    kprintf("inside read syscall\n");
    return 0;
}


int
sys_write(int fd, const void *buf, size_t nbytes)
{
    kprintf("inside write syscall\n");
    return 0;
}


int
sys_lseek(int fd,  off_t offset, int whence)
{
    kprintf("inside lseek syscall\n");
    return 0;
}


int
sys_close(int fd)
{
    kprintf("inside close syscall\n");
    return 0;
}


int
sys_dup2(int oldfd, int newfd)
{
    kprintf("inside dup2 syscall\n");
    return 0;
}
