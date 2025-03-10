#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  buf[DIRSIZ] = 0;
  return buf;
}

void
find(char *path, char *target, int type_flag, char type_val, int inum_flag, int inum_val, int printi_flag)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    if(type_flag && type_val == 'f' || inum_flag && st.ino == inum_val){
      char *name = fmtname(path);
      if(strcmp(name, target) == 0){
        if(printi_flag)
          printf("%d ", st.ino);
        printf("%s\n", path);
      }
    }
    break;

  case T_DIR:
    if(type_flag && type_val == 'd'){
      char *name = fmtname(path);
      if(strcmp(name, target) == 0){
        if(printi_flag)
          printf("%d ", st.ino);
        printf("%s\n", path);
      }
    }
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      find(buf, target, type_flag, type_val, inum_flag, inum_val, printi_flag);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: find path -name filename [-type f|d] [-inum n] [-printi]\n");
    exit(1);
  }

  char *path = argv[1];
  int i = 2;
  char *target = 0;
  int type_flag = 0;
  char type_val = 0;
  int inum_flag = 0;
  int inum_val = 0;
  int printi_flag = 0;

  while(i < argc){
    if(strcmp(argv[i], "-name") == 0){
      if(i + 1 >= argc){
        fprintf(2, "find: missing argument to -name\n");
        exit(1);
      }
      target = argv[i + 1];
      i += 2;
    }
    else if(strcmp(argv[i], "-type") == 0){
      if(i + 1 >= argc){
        fprintf(2, "find: missing argument to -type\n");
        exit(1);
      }
      type_flag = 1;
      type_val = argv[i + 1][0];
      if(type_val != 'f' && type_val != 'd'){
        fprintf(2, "find: invalid argument to -type\n");
        exit(1);
      }
      i += 2;
    }
    else if(strcmp(argv[i], "-inum") == 0){
      if(i + 1 >= argc){
        fprintf(2, "find: missing argument to -inum\n");
        exit(1);
      }
      inum_flag = 1;
      inum_val = atoi(argv[i + 1]);
      i += 2;
    }
    else if(strcmp(argv[i], "-printi") == 0){
      printi_flag = 1;
      i++;
    }
    else {
      fprintf(2, "find: unknown option %s\n", argv[i]);
      exit(1);
    }
  }

  if(target == 0){
    fprintf(2, "find: -name option required\n");
    exit(1);
  }

  find(path, target, type_flag, type_val, inum_flag, inum_val, printi_flag);
  exit(0);
}

write(STDERR_FILENO, error_message, strlen(error_message));