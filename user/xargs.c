//implementation of xargs

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
   char params[MAXARG][30]; 			//2D array for whole single words created by characters 
   int index = 0;
   for(index=0; index<argc-1; index++){
   	strcpy(params[index],argv[index+1]);		//copying arguments from cmd to array
   }
   
   char c;
   int word=0;
   while(read(0,&c,1)>0){			//reading char by char until nothing left
   	if(c == '\n'){				//after new line word is closed by \0 character,new parameter
            params[index][word]= '\0';
            word=0;
            index++;
   	}else{
   	    params[index][word] = c;
   	    word++;
   	}
   }   
   char *pExec[index+1];			//back from 2D array to classic array 
   for(int j=0; j<index; j++){
   	pExec[j]=params[j];
   }
   
   int pid =fork();
   
   if(pid == 0){
   exec(argv[1],pExec);
   }else if(pid>0){
   	wait(0);
   }else{
   	fprintf(2,"error : negative pid : %d\n",getpid());
   }
   
   exit(0);
}
