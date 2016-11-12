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
sys_fork(struct *parent_tf, int *retval)
{
    // kprintf("inside fork syscall\n");
    // int mybaby = 0;
    // struct thread *mybaby_thread;
    // //copy  parent tf
    // struct trapframe *mybaby_tf = kmalloc(siszeof(struct trapframe));
    // if(mybaby_tf == NULL){
    //   kprintf("unabled to make tf for my baby ;_; \n");
    //   return ENOMEM;
    // }
    // *mybaby_tf = *parent_tf;
    // //make room for my baby
    // struct addrspace *mybaby_room;
    // mybaby = as_copy(curthread->t_vmspace, &mybaby_room);
    // //mybaby is okay if is 0...idk why
    // if(mybaby){
    //   kprintf("my baby has no room!!!");
    //   return ENOMEM;
    // }
    // // my baby has trapframe and room!!!!

    // //monkey sees monkey dooo, learn from your parent, son!
    // //md_forkentry is the entry point for child thread
    // mybaby = thread_fork("my son"),  mybaby_tf,mybaby_room,md_forkentry, retval);

    // if(mybaby){
    //   kprintf("ABORTTTEDDD: cant fork thread for the child...\n");
    //   kfree(mybaby);
    //   return result;
    // }

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
    curthread->exit_status = 0;
    thread_exit();
    return 0;
}
