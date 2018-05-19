#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

int (*real_statfs)(const char *path, struct statfs *buf);
int (*real_statfs64)(const char *path, struct statfs64 *buf);
int (*real_statvfs)(const char *path, struct statvfs *buf);

int (*real_fstatfs)(int fd, struct statfs *buf);
int (*real_fstatfs64)(int fd, struct statfs64 *buf);
int (*real_fstatvfs)(int fd, struct statvfs *buf);

int (*real_setenv)(const char *name, const char *value, int overwrite);
int (*real_unsetenv)(const char *name);
int (*real_putenv)(char *string);


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

  fprintf(stderr,"INTERCEPTED statfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int statfs64(const char *path, struct statfs64 *buf){
  real_statfs64 = dlsym(RTLD_NEXT,"statfs64");
  int ret = real_statfs64(path,buf);

  fprintf(stderr,"INTERCEPTED statfs64  with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int statvfs(const char *path, struct statvfs *buf){
  real_statvfs = dlsym(RTLD_NEXT,"statvfs");
  int ret = real_statvfs(path,buf);

  fprintf(stderr,"INTERCEPTED statvfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int fstatfs(int fd, struct statfs *buf){
  real_fstatfs = dlsym(RTLD_NEXT,"fstatfs");
  int ret = real_fstatfs(fd,buf);

  fprintf(stderr,"INTERCEPTED fstatfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int fstatfs64(int fd, struct statfs64 *buf){
  real_fstatfs64 = dlsym(RTLD_NEXT,"fstatfs64");
  int ret = real_fstatfs64(fd,buf);

  fprintf(stderr,"INTERCEPTED fstatfs64 with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}


int fstatvfs(int fd, struct statvfs *buf){
  real_fstatvfs = dlsym(RTLD_NEXT,"fstatvfs");
  int ret = real_fstatvfs(fd,buf);

  fprintf(stderr,"INTERCEPTED fstatfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}


int setenv(const char *name, const char *value, int overwrite){
  real_setenv = dlsym(RTLD_NEXT,"setenv");
  if(0==strcmp(name,"LD_PRELOAD")){
    fprintf(stderr,"PREVENTED setenv '%s' = '%s'\n",name,value);
    return 0;
  }
  fprintf(stderr,"ALLOWED setenv '%s' = '%s'\n",name,value);
  return real_setenv(name,value,overwrite);
}

int unsetenv(const char *name){
  real_unsetenv = dlsym(RTLD_NEXT,"unsetenv");
  if(0==strcmp(name,"LD_PRELOAD")){
    fprintf(stderr,"PREVENTED unsetenv '%s'\n",name);
    return 0;
  }
  fprintf(stderr,"ALLOWED unsetenv '%s'\n",name);
  return real_unsetenv(name);
}

int clearenv(void){
  fprintf(stderr, "PREVENTED clearenv\n");
}

int putenv(char *string){
  real_putenv = dlsym(RTLD_NEXT,"putenv");
  char *buf = malloc(strlen(string)*sizeof(char));
  strcpy(buf,string);
  char *name = strtok(name,"=");
  if(0==strcmp(name,"LD_PRELOAD")){
    fprintf(stderr,"PREVENTED putenv '%s'\n",name);
    return 0;
  }
  fprintf(stderr,"ALLOWED putenv '%s'\n",name);
  free(buf);
  free(name);
  return real_putenv(string);
}
