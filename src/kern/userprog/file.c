#include <types.h>
#include <lib.h>
#include <synch.h>
#include <array.h>
#include <vfs.h>
#include <vnode.h>
#include <fs.h>
#include <uio.h>
#include <curthread.h>
#include <kern/limits.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <thread.h>
#include <file.h>
#include <kern/stat.h>
//int sys_open(const char *path, int oflag, mode_t mode)
int
sys_open(const char *path, int oflag, int mode)
{
    kprintf("inside open syscall\n");

    struct vnode *vn;
    struct openFile *file;
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

    // //check valid access mode
    // KASSERT(file->fMode==O_RDONLY || file->fMode==O_WRONLY || file->fMode==O_RDWR);

    //Place file in fileTable
    struct fileTable *ft = curthread->t_fileTable;
    int fileDescriptor;
    bool added = false; //bool that indicates if the file has been added to table
    int i;
    //Find room in file table
    for (i = 0; i < MAX_OPEN_FILES; i = i + 1) {
        if (ft->tOpenfiles[i] == NULL) {
            //Adding the file
            ft->tOpenfiles[i] = file;
            fileDescriptor = i;
            added = true;
            kprintf("File added to table :D ");
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
        kprintf("File table is FULL :(");
        return EMFILE;
    }

}


int
sys_read(int fd, void *buf, size_t nbytes)
{
    kprintf("inside read syscall\n");
    //Oh boi here we go
    struct openFile file;
    
    //check valid fd
    if (fd < 0) {
        return EBADF;
    }
    struct fileTable *ft = curthread->t_fileTable;
    //check valid fileTable
    if (ft == NULL) {
        return EBADF;
    }
    file = ft->tOpenfiles[fd];
    //Check if file can be read
    if (file->fMode == O_RDONLY || file->fMode == O_RDWR) {
        return EINVAL;
    }
    lock_acquire(file->fLock);

    //Get a read uio
    struct uio readUio;
    // struct iovec readIovec;
    char *buffer = (char*)kmalloc(nbytes);
    mk_kuio(&readUio, (void*)buffer, nbytes, file->offset, UIO_READ);

    int result = VOP_READ(file->vnode, &readUio);
    file->offset = readUio.uio_offset;
    kfree(buffer);
    lock_release(file->fLock);
    //See if the read worked
    if(result) {
        return result;
    }
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
