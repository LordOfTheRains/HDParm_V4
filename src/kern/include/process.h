#ifndef _PROCESS_H_
#define _PROCESS_H_

/*
 * Prototypes for IN-KERNEL entry points for system process call implementations.
 */

int sys_getpid(pid_t *retval);

int sys_fork(struct trapframe *parent_tf, int *retval);

int sys_execv(const char *path, const char *argv[]);

int sys_waitpid(pid_t wpid, int *status, int options);

int sys_exit();

#endif /* _PROCESS_H_ */
