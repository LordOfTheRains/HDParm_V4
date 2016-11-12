#ifndef _FILE_H_
#define _FILE_H_

#include <vnode.h>
#include <types.h>
#include <lib.h>
#include <synch.h>
#include <uio.h>
#include <current.h>
#include <vfs.h>
#include <file.h>
#include <syscall.h>
#include <kern/errno.h>
#include <kern/limits.h>
#include <kern/stat.h>
#include <kern/unistd.h>
#include <kern/fcntl.h>

#define MAX_OPEN_FILES = 1;
/*
* I'm used to boolean variables and when I don't have them
* I feel lost and confused. So I made my own.
*/
typedef int bool;
enum { false, true };

struct vnode;



/*
* Structure that holds the metadata of an open file
*/
struct openFile {
  struct vnode *fVnode;
  struct lock *fLock;
  int fMode;
  int fRefCount;
  off_t fOffset;
};

/*
* Structure that holds a list of all currently open files
*/
struct fileTable {
  struct openFile *tOpenfiles[MAX_OPEN_FILES];
};

int sys_open(const char *path, int oflag, int mode);

int sys_read(int fd, void *buf, size_t nbytes);

int sys_write(int fd, const void *buf, size_t nbytes);

int sys_lseek(int fd,  off_t offset, int whence);

int sys_close(int fd);

int sys_dup2(int oldfd, int newfd);

#endif /* FILE */
