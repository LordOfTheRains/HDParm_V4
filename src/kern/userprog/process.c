#include <types.h>
#include <process.h>
#include <syscall.h>
#include <thread.h>
#include <curthread.h>
#include <pid_factory.h>

#include <kern/errno.h>
/*sample implementation of sys_getpid() */

int
sys_getpid(pid_t *retval)
{
    *retval = curthread->t_pid;
    return 0;
}

int
sys_fork(struct trapframe *parent_tf, int *retval)
{
    kprintf("inside fork syscall\n");
    int mybaby = 0;
    struct thread *mybaby_thread;
    //copy  parent tf
    struct trapframe *mybaby_tf = kmalloc(sizeof(struct trapframe));
    if(mybaby_tf == NULL){
      kprintf("unabled to make tf for my baby ;_; \n");
      return ENOMEM;
    }
    *mybaby_tf = *parent_tf;
    //make room for my baby
    struct addrspace *mybaby_room;
    mybaby = as_copy(curthread->t_vmspace, &mybaby_room);
    //mybaby is okay if is 0...idk why
    if(mybaby){
      kprintf("my baby has no room!!!");
      return ENOMEM;
    }
    // my baby has trapframe and room!!!!

    //monkey sees monkey dooo, learn from your parent, son!
    //md_forkentry is the entry point for child thread
    mybaby = thread_fork("my son",  mybaby_tf, (unsigned long) mybaby_room,goto_new_process, &mybaby_thread);

    if(mybaby){
      kprintf("ABORTTTEDDD: cant fork thread for the child...\n");
      kfree(mybaby);
      return result;
    }
    *retval = mybaby-> t_pid;
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
    if((int)wpid >= 50 || (int)wpid <0 ){
      kprintf("pid does not exist!\n");
      thread_panic();
    }
    struct pidinfo = get_pidinfo(wpid);
    if (pidinfo->exited){
      return pidinfo->exit_status;
    }
    else {
      return -1;
    }


}


int
sys_exit()
{
    kprintf("inside _exit syscall\n");
    //modify pid exit status
    pid_t *pid = curthread->t_pid;
    struct pidinfo = get_pidinfo(pid);
    pidinfo->exit_status = 0;
    thread_exit();
    return 0;
}

void goto_new_process(void * data1, unsigned long data2){
  struct trapframe *tf = (struct trapframe) data1;
  struct addrspace *addr = (struct addrspace) data2;

   struct trapframe trapstack;
   trapstack = *tf;
   trapstack->tf_v0  = 0;
   trapstack->tf_a3 = 0;
   trapstack->epc += 4;

   as_copy(addr, &curthread->t_vmspace);
   as_activate(curthread->t_vmspace);
   //
   	kprintf("going into userland");
   mips_usermode(&trapstack);
}
