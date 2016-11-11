#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <array.h>
#include <machine/spl.h>
#include <machine/pcb.h>
#include <thread.h>
#include <curthread.h>
#include "<synch.h"
#include <scheduler.h>
#include <addrspace.h>
#include <vnode.h>
#include <pid_factory.h>

struct pidinfo{
  int my_pid;
  int dad_pid;
  struct cv *my_cv;
  int exited;
  int exit_status;
}

//make var to save pidInfo
//static struct pidInfo
static int MAX_PIDS = 50;
static struct lock *pid_lock;
static struct lock *free_pid_lock;
static struct lock *pid_count_lock;
//table of all pid
static struct pidinfo *pidinfo[MAX_PIDS];
static int pid_count; //keep track of how many pid are being used
// this available_pids[i] == 1 means available, 0 means not
static int  *available_pids[MAX_PIDS]; //keep track of available pid


void pid_init(){
  pid_count = 0;
  pid_lock = lock_create("pidinfo_table_lock");
  free_pid_lock = lock_create("available_pids_lock");
  pid_count_lock = lock_create("pid_count_lock");

}


pid_t pid_create(){
  struct pidinfo *mypid;
  lock_acquire(pid_count_lock);
  if(pid_count >= MAX_PIDS){
    lock_release(pid_count_lock);
    kprintf("max number of proc reached! \n");
    return NULL;
  }
  lock_release(pid_count_lock);
  mypidinfo = kmalloc(sizeof(struct pidinfo));
  pidcv = kmalloc(sizeof(struct cv));
  if (mypidinfo == NULL || pidcv == NULL ){
      kfree(mypidinfo);
      kfree(pidcv);
      kprintf ("unable to make room for pid info or cv.\n");
      return ENOMEM;
  }
  //set the pids
  mypid->dad_pid = curthread->t_pid;
  pid_t new_pid = get_available_pid();
  if (new_pid == NULL) {
    kprintf("unable to find avaiable pid to use");
  }
  mypid->my_pid = new_pid;
  lock_acquire(pid_count_lock);
  pid_count++;
  lock_release(pid_count_lock);
  return new_pid;
}

pid_t get_available_pid(){
  lock_acquire(free_pid_lock);
  for (int i = 0; i < MAX_PIDS; i++){
    if (available_pids[i]) {
      pid_t available_pid = i;
      return available_pid;//using i as pid
    }
  }
  lock_release (free_pid_lock);
  return NULL;
}



void pid_destroy (pid_t pid) {
  //destroy pid and pidinfo
  //removes it from the mem space
  //set the pid to avaiable
  lock_acquire(pid_count_lock);
  pid_count--;
  lock_release(pid_count_lock);
  lock_acquire(free_pid_lock);
  available_pids[pid] = 0;
  lock_release(free_pid_lock);
}


void get_pidinfo(pid_t pid){
  return pidinfo[pid];
}
