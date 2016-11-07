#ifndef _FILE_H_
#define _FILE_H_


int sys_open(const char *path, int oflag, int mode);

int sys_read(int fd, void *buf, size_t nbytes);

int sys_write(int fd, const void *buf, size_t nbytes);

int sys_lseek(int fd,  off_t offset, int whence);

int sys_close(int fd);

int sys_dup2(int oldfd, int newfd);

#endif /* FILE */
