#include <types.h>
#include <lib.h>
#include <synch.h>
#include <uio.h>
#include <thread.h>
#include <current.h>
#include <vfs.h>
#include <vnode.h>
#include <file.h>
#include <syscall.h>

//int sys_open(const char *path, int oflag, mode_t mode)
int
sys_open(const char *path, int oflag, int mode)
{
    kprintf("inside open syscall\n");

    struct vnode *vn;
    struct openFile file;
    int err;

    //Use vfs_open and check for errors. If error, return.
    err = vfs_open(path, oflag, &vn);
    if (err) {
        return err;
    }
    
    kmalloc(sizeof(struct openFile));
    // if (file==NULL) {
    //     vfs_close(vn);
    //     return ENOMEM;
    // }

    //init file
    file->fLock     = lock_create("file lock");
    if (file->fLock==NULL ) {
        vfs_close(vn);
        kfree(file);
        return ENOMEM;
    }

    file->fVnode    = vn;
    file->fMode     = mode;
    file->fRefCount = 1;
    file->fOffset   = 0;

    //check valid access mode
    KASSERT(file->fMode==O_RDONLY || file->fMode==O_WRONLY || file->fMode==0_RDWR);

    //Place file in fileTable
    struct fileTable *ft = curthread->t_filetable;
    int i;
    bool added = false; //bool that indicates if the file has been added to table
    for (i = 0; i < MAX_OPEN_FILES; i++) {
        if (ft->tOpenfiles[i] == NULL) {
            //fd = i;
            added = true;
        }
    }

    //If file successfully added, return 0.
    //else return EMFILE error
    if (added) {
        lock_destroy(file->fLock);
        kfree(file);
        vfs_close(vn);
        return 0;
    } else {
        return EMFILE;
    }

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
