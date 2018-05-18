#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <dlfcn.h>

int (*real_statfs)(const char *path, struct statfs *buf);
int (*real_statfs64)(const char *path, struct statfs64 *buf);
int (*real_statvfs)(const char *path, struct statvfs *buf);

int (*real_fstatfs)(int fd, struct statfs *buf);
int (*real_fstatfs64)(int fd, struct statfs64 *buf);
int (*real_fstatvfs)(int fd, struct statvfs *buf);

#define MAKE_SMALL(name)   buf->name = (buf->name & 0x0fffffff);
#define MAKE_SMALLER(name) buf->name = (buf->name & 0x00ffffff);

#define MAKE_ALL_SMALLER \
  MAKE_SMALL  (f_blocks) \
  MAKE_SMALLER(f_bfree)  \
  MAKE_SMALLER(f_bavail) \
  MAKE_SMALL  (f_files)  \
  MAKE_SMALLER(f_ffree)


int statfs(const char *path, struct statfs *buf){
  real_statfs = dlsym(RTLD_NEXT,"statfs");
  int ret = real_statfs(path,buf);

  printf("INTERCEPTED statfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  printf("                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int statfs64(const char *path, struct statfs64 *buf){
  real_statfs64 = dlsym(RTLD_NEXT,"statfs64");
  int ret = real_statfs64(path,buf);

  printf("INTERCEPTED statfs64  with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  printf("                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int statvfs(const char *path, struct statvfs *buf){
  real_statvfs = dlsym(RTLD_NEXT,"statvfs");
  int ret = real_statvfs(path,buf);

  printf("INTERCEPTED statvfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  printf("                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int fstatfs(int fd, struct statfs *buf){
  real_fstatfs = dlsym(RTLD_NEXT,"fstatfs");
  int ret = real_fstatfs(fd,buf);

  printf("INTERCEPTED fstatfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  printf("                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int fstatfs64(int fd, struct statfs64 *buf){
  real_fstatfs64 = dlsym(RTLD_NEXT,"fstatfs64");
  int ret = real_fstatfs64(fd,buf);

  printf("INTERCEPTED fstatfs64 with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  printf("                      new bfree = %d\n",buf->f_bfree);

  return ret;
}


int fstatvfs(int fd, struct statvfs *buf){
  real_fstatvfs = dlsym(RTLD_NEXT,"fstatvfs");
  int ret = real_fstatvfs(fd,buf);

  printf("INTERCEPTED fstatfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  printf("                      new bfree = %d\n",buf->f_bfree);

  return ret;
}
