// implementation of finding prime numbers between 2-35 by pipes
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void findingPrime(int parentNumber){
  int prime;
  if(read(parentNumber,&prime,sizeof(int))){	//read allow to write another number to parent, so 
      int to_child[2];				//when last number is added statement is false
      pipe(to_child);

      int pid = fork();
      int received;

      if(pid == 0){
        close(to_child[0]);
        while(read(parentNumber,&received,sizeof(int))){	//storing value to recieve and 
	if(received % prime != 0){				//checking if number is prime
	  write(to_child[1], &received, sizeof(int));
	}
      }
      close(to_child[1]);
      }else if(pid > 0){
        close(to_child[1]);
	printf("prime %d\n",prime);
	findingPrime(to_child[0]);	//recursively finding prime 
      }else{								
	fprintf(2,"error : negative pid : %d\n",getpid());
      }
  }
}

void parentProcess(int to_parent[2],int pid){
    if(pid==0){
    	close(to_parent[0]);
    	for(int i = 2;i <=35; i++){
   	   write(to_parent[1],&i,sizeof(int));	//stacking values in parent 
    }
    close(to_parent[1]);
    }else if(pid>0){
      	close(to_parent[1]);
      	findingPrime(to_parent[0]);
    }else{
   	 fprintf(2,"error : negative pid : %d\n",getpid());
  }	
}

int main(int argc, char *argv[]){
  if(argc>1){
  	fprintf(2,"argument is not expected\n");
  	exit(0);
  }
  int to_parent[2];
  pipe(to_parent);

  int pid = fork();

  parentProcess(to_parent,pid);

  exit(0);
}
