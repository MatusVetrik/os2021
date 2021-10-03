// implementation of script to find path to file (source ls.c)
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *dirName, char *fileName ){
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512], *p;

  if((fd=open(dirName, 0))<0){
    fprintf(2, "find: cannot open %s\n",dirName);
    exit(0);
  }

  if(fstat(fd,&st) < 0){
    fprintf(2, "find: cannot stat %s\n", dirName);
    close(fd);
    exit(0);
  }

  if(st.type!=T_DIR){
    fprintf(2, "find: you are not in directory\n");
    exit(0);
  }

  strcpy(buf, dirName);		
  p = buf+strlen(buf);
  *p++ = '/';			//copying directory name to buffer("path") and adding "/" to path

  while(read(fd, &de,sizeof(de)) == sizeof(de)){
    if(de.inum == 0)
      continue;
    if(!strcmp(de.name,".")||!strcmp(de.name,"..")) 	//ignoring characters . AND .. to avoid looping
      continue;

    memmove(p,de.name,DIRSIZ);	// copying DIRSIZ characters from de.name to p 
    p[DIRSIZ] = 0;

    if(stat(buf,&st) < 0){
      fprintf(2,"find: cannot stat %s\n",buf);
      continue;
    }

    if (!strcmp(de.name,fileName)){	//system is writing path while we are in directory not file
      printf("%s\n",buf);
    }

    else if(st.type==T_DIR){	//recursion while we are still in the directory not file
      find(buf,fileName);
    }
  }
}
int main(int argc, char *argv[]){
  if(argc > 3){
  	fprintf(2,"too many arguments, expected 2 arguments: dirName and fileName\n");
  	exit(0);
  }
  
  find(argv[1],argv[2]);
  exit(0);
}

