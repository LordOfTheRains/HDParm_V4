#include <types.h>
#include <process.h>
#include <syscall.h>
#include <thread.h>
#include <curthread.h>

/*sample implementation of sys_getpid() */

int
sys_getpid(pid_t *retval)
{
    *retval = curthread->t_pid;
    return 0;
}

int
sys_fork()
{
    kprintf("inside fork syscall\n");
    return 0;
}


int
sys_execv(const char *path, const char *argv[])
{
    kprintf("inside execv syscall\n");
    return 0;
}

int
sys_waitpid(pid_t wpid, int *status, int options)
{
    kprintf("inside waitpid syscall\n");
    return 0;
}


int
sys_exit()
{
    kprintf("inside _exit syscall\n");
    return 0;
}
