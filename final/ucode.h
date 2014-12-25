/* 
 * File:   ucode.h
 * Author: ericfoss
 *
 * Created on December 2, 2014, 11:10 AM
 */

#ifndef UCODE_H
#define	UCODE_H

#include "my_assert.h"
#include "printlib.h"
#include "type.h"

int get_line(int fd, char *buf, int len, int *eof);
int getsecret(char *buf, int len);
int strncmp(char *str1, char *str2, int i);
int getpid();
int getppid();
int getpri();
int chpri(int value);
int getuid();
int chuid(int uid, int gid);
int tswitch();
int fork();
int exec(char *cmd_line);
int wait(int *status);
int vfork();
int thread(int fn, int stack, int flag, int ptr);
int mutex_creat();
int mutex_lock(int *m);
int mutex_unlock(int *m);
int mutex_destroy(int *m);
int mkdir(char *name);
int rmdir(char *name);
int creat(char *filename);
int link(char *oldfile, char *newfile);
int unlink(char *file);
int symlink(char *oldfile, char *newfile);
int readlink(char *file, char *linkname);
int chdir(char *name);
int getcwd(char *cwdname);
int stat(char *filename, struct stat *sPtr);
int fstat(int fd, struct stat *sptr);
int open(char *file, int flag);
int close(int fd);
u32 lseek(int fd, u32 offset, int ww);
int read(int fd, char *buf, int nbytes);
int write(int fd, char *buf, int nbytes);
int pipe(int *pd);
int chmod(char *file, u16 mode);
int chown(char *file, int uid);
int touch(char *filename);
int fixtty(char *tty);
int settty(char *tty);
int gettty(char *tty);
int dup(int fd);
int dup2(int fd, int gd);
int ps(char *y);
int mount(char *dev, char **mpt);
int umount(char *dev);
int getSector(u32 sector, char *ubuf, u16 nsector);
int do_cmd(u16 cmd, u16 value);
int kill(int sig, int pid);
int signal(u16 sig, u16 catcher);
int pause(u16 t);
int itimer(u16 t);
int send(char *msg, int pid);
int recv(char *msg);
int do_texit();
int tjoin(int n);
int texit(int v);
int khits(int *requests, int *hits);
int setcolor(int color);
int sync();
int thinit();
int exit(int value);
int pwd();
void flush(int fd);

#endif	/* UCODE_H */

