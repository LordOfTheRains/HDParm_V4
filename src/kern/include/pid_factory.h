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



struct pidinfo{
  int my_pid;
  int dad_pid;
  struct cv *my_cv;
  int exited;
  int exit_status;
}


static int MAX_PIDS = 50;
static struct lock *pid_lock;
static struct lock *free_pid_lock;
static struct lock *pid_count_lock;
//table of all pid
static struct pidinfo *pidinfo[MAX_PIDS];
static int pid_count; //keep track of how many pid are being used
// this available_pids[i] == 1 means available, 0 means not
static int  *available_pids[MAX_PIDS]; //keep track of available pid


void pid_init();
pid_t pid_create();
pid_t get_available_pid();
void pid_destroy(pid_t pid);
static struct pidinfo* get_pidinfo(pid_t pid);
