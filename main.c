#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

__attribute__((constructor)) static void swfsoverflow_init(void);

int (*real_statfs)(const char *path, struct statfs *buf);
int (*real_statfs64)(const char *path, struct statfs64 *buf);
int (*real_statvfs)(const char *path, struct statvfs *buf);

int (*real_fstatfs)(int fd, struct statfs *buf);
int (*real_fstatfs64)(int fd, struct statfs64 *buf);
int (*real_fstatvfs)(int fd, struct statvfs *buf);

int (*real_setenv)(const char *name, const char *value, int overwrite);
int (*real_unsetenv)(const char *name);
int (*real_putenv)(char *string);

int (*real_execve)(const char *filename, char *const argv[], char *const envp[]);

char *path;

#define MAKE_SMALL(name)   buf->name = (buf->name & 0x0fffffff);
#define MAKE_SMALLER(name) buf->name = (buf->name & 0x00ffffff);

#define MAKE_ALL_SMALLER \
  MAKE_SMALL  (f_blocks) \
  MAKE_SMALLER(f_bfree)  \
  MAKE_SMALLER(f_bavail) \
  MAKE_SMALL  (f_files)  \
  MAKE_SMALLER(f_ffree)

static void swfsoverflow_init(void){
  char *_path = getenv("LD_PRELOAD");
  path = malloc((strlen(_path)+1)*sizeof(char));
  strcpy(path,_path);
}

int statfs(const char *path, struct statfs *buf){
  if(real_statfs == NULL) real_statfs = dlsym(RTLD_NEXT,"statfs");
  int ret = real_statfs(path,buf);

  fprintf(stderr,"INTERCEPTED statfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int statfs64(const char *path, struct statfs64 *buf){
  if(real_statfs64 == NULL) real_statfs64 = dlsym(RTLD_NEXT,"statfs64");
  int ret = real_statfs64(path,buf);

  fprintf(stderr,"INTERCEPTED statfs64  with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int statvfs(const char *path, struct statvfs *buf){
  if(real_statvfs == NULL) real_statvfs = dlsym(RTLD_NEXT,"statvfs");
  int ret = real_statvfs(path,buf);

  fprintf(stderr,"INTERCEPTED statvfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int fstatfs(int fd, struct statfs *buf){
  if(real_fstatfs == NULL) real_fstatfs = dlsym(RTLD_NEXT,"fstatfs");
  int ret = real_fstatfs(fd,buf);

  fprintf(stderr,"INTERCEPTED fstatfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}

int fstatfs64(int fd, struct statfs64 *buf){
  if(real_fstatfs64 == NULL) real_fstatfs64 = dlsym(RTLD_NEXT,"fstatfs64");
  int ret = real_fstatfs64(fd,buf);

  fprintf(stderr,"INTERCEPTED fstatfs64 with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}


int fstatvfs(int fd, struct statvfs *buf){
  if(real_fstatvfs == NULL) real_fstatvfs = dlsym(RTLD_NEXT,"fstatvfs");
  int ret = real_fstatvfs(fd,buf);

  fprintf(stderr,"INTERCEPTED fstatfs   with bfree = %d\n",buf->f_bfree);

  MAKE_ALL_SMALLER

  fprintf(stderr,"                      new bfree = %d\n",buf->f_bfree);

  return ret;
}


int setenv(const char *name, const char *value, int overwrite){
  if(real_setenv == NULL) real_setenv = dlsym(RTLD_NEXT,"setenv");
  if(0==strcmp(name,"LD_PRELOAD")){
    fprintf(stderr,"PREVENTED setenv '%s' = '%s'\n",name,value);
    return 0;
  }
  fprintf(stderr,"ALLOWED setenv '%s' = '%s'\n",name,value);
  return real_setenv(name,value,overwrite);
}

int unsetenv(const char *name){
  if(real_unsetenv == NULL) real_unsetenv = dlsym(RTLD_NEXT,"unsetenv");
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
  char *buf = malloc((strlen(string)+1)*sizeof(char));
  strcpy(buf,string);
  char *name = strtok(buf,"=");
  if(0==strcmp(name,"LD_PRELOAD")){
    fprintf(stderr,"PREVENTED putenv '%s'\n",name);
    return 0;
  }
  fprintf(stderr,"ALLOWED putenv '%s'\n",name);
  free(buf);
  free(name);
  return real_putenv(string);
}

int execve(const char *filename, char *const argv[], char *const envp[]){
  if(real_execve == NULL) real_execve = dlsym(RTLD_NEXT,"execve");
  fprintf(stderr, "INTERCEPTED execve, env:\n");
  int i;
  for(i=0;envp[i]!=NULL;i++);
  char *nenvp[i+1];
  nenvp[i]=NULL;
  for(i=0;envp[i]!=NULL;i++){
    char *string = envp[i];
    char *buf = malloc((strlen(string)+1)*sizeof(char));
    strcpy(buf,string);
    char *name = buf;
    char *sep = strchr(name, '=');
    if(NULL!=sep){
      *sep = '\0';
    }
    if((NULL!=sep) && (0==strcmp(name,"LD_PRELOAD"))){
      char *value = sep+1;
      fprintf(stderr,"  FIXING '%s'\n",string);
      char * nstring = malloc((strlen(name)+strlen(path)+strlen(value)+3)*sizeof(char));
      strcpy(nstring,name);
      strcat(nstring,"=");
      strcat(nstring,path);
      strcat(nstring,":");
      strcat(nstring,value);
      nenvp[i]=nstring;
      fprintf(stderr,"    TO  '%s'\n",nenvp[i]);
    }else{
      nenvp[i]=envp[i];
      fprintf(stderr,"  LEFT  '%s'\n",nenvp[i]);
    }
    free(buf);
  }
  free(path);
  fprintf(stderr, "  CALLING %s\n", filename);
  return real_execve(filename,argv,nenvp);
}