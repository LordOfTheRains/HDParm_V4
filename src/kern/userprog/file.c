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
    struct openFile *file;
    int check = check_valid(fd); 
    if (check) {
        return check;
    }

    struct fileTable *ft = curthread->t_fileTable;

    file = ft->tOpenfiles[fd];
    //Check if file can be read
    if (file->fMode == O_WRONLY) {
        return EINVAL;
    }
    lock_acquire(file->fLock);

    //Get a read uio
    struct uio readUio;
    char *buffer = (char*)kmalloc(nbytes);
    mk_kuio(&readUio, (void*)buffer, nbytes, file->fOffset, UIO_READ);

    int result = VOP_READ(file->fVnode, &readUio);
    file->fOffset = readUio.uio_offset;
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
    int check = check_valid(fd); 
    if (check) {
        kprintf("File invalid\n");
        return check;
    }
    struct openFile *file;
    struct fileTable *ft = curthread->t_fileTable;

    file = ft->tOpenfiles[fd];
    if (file->fMode == O_RDONLY) {
        kprintf("File can't be written\n");
        return EINVAL;
    }
    lock_acquire(file->fLock);

    struct uio writeUio;
    char *buffer = (char*)kmalloc(nbytes);
    mk_kuio(&writeUio, (void*)buffer, nbytes, file->fOffset, UIO_WRITE);

    int result = VOP_WRITE(file->fVnode, &writeUio);
    file->fOffset = writeUio.uio_offset;
    kfree(buffer);
    lock_release(file->fLock);
    if (result) {
        kprintf("file written successfully\n");
        return result;
    }
    kprintf("file not written\n");
    return 0;
}


int
sys_lseek(int fd,  off_t offset, int whence)
{
    kprintf("inside lseek syscall\n");
    int check = check_valid(fd); 
    if (check) {
        kprintf("File invalid. Error: %d\n", check);
        return check;
    }
    struct fileTable *ft = curthread->t_fileTable;
    struct openFile *file = ft->tOpenfiles[fd];

    lock_acquire(file->fLock);

    int newOffset;
    int result;
    struct stat fileStat;

    //switchy
    switch(whence) {
        case SEEK_SET:
            kprintf("SEEK_SET\n");
            newOffset = offset;
            break;

        case SEEK_CUR:
            kprintf("SEEK_CUR\n");
            newOffset = file->fOffset + offset;
            break;

        case SEEK_END:
            kprintf("SEEK_END\n");
            //use VOP_STAT to get a stat struct for the file
            result = VOP_STAT(file->fVnode, &fileStat);
            //make sure everything is kosher
            if (result) {
                kprintf("VOP_STAT call failed. Error: %d\n", result);
                return result;
            }
            newOffset = fileStat.st_size + offset;
            break;

        default:
            kprintf("whence defaulted\n");
            lock_release(file->fLock);
            return EINVAL;
    }

    //Make sure newOffset is legit
    if (newOffset < 0) {
        kprintf("offset < 0\n");
        lock_release(file->fLock);
        return EINVAL;
    }

    //Seek enlightenment
    result = VOP_TRYSEEK(file->fVnode, newOffset);
    lock_release(file->fLock);
    //see if it worked
    if (result) {
        kprintf("lseek failed. Error: %d", result);
        return result;
    }
    kprintf("lseek worked, yay!\n");
    return newOffset;
}


int
sys_close(int fd)
{
    kprintf("inside close syscall\n");
    int check = check_valid(fd);
    if (check) {
        return check;
    }
    //GET DEM FILES 
    struct fileTable *ft = curthread->t_fileTable;
    struct openFile *file = ft->tOpenfiles[fd];

    lock_acquire(file->fLock);
    
    vfs_close(file->fVnode);

    if (file->fRefCount == 0) {
        kfree(file->fVnode);
        kfree(ft->tOpenfiles[fd]);
    }
    ft->t_fileTable[fd] = NULL;
    lock_release(file->fLock);
    return 0;
}


int
sys_dup2(int oldfd, int newfd)
{
    kprintf("inside dup2 syscall\n");
    return 0;
}

int
check_valid(int *fd) {
    //check valid fd
    if (fd < 0) {
        return EBADF;
    }
    //check valid fileTable
    if (curthread->t_fileTable == NULL) {
        return EBADF;
    }
    return 0;
}
