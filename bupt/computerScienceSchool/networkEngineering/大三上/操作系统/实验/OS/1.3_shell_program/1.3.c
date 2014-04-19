#include<stdio.h>
#include<sys/wait.h> 
int main(){
	printf("Hello Linux\n");
	int pid;
	int state;
	int pfd[2];
	pipe(pfd);
	if (fork()==0){
		printf("In the grep progress\n");
		dup2(pfd[0],0);
		close(pfd[0]);
		close(pfd[1]);
		execlp("grep","grep","sh",0);
		perror("exelp grep error");
	}
	else if(fork()==0){
	    printf("In the ps progress\n");
		dup2(pfd[1],1);
		close(pfd[0]);
		close(pfd[1]);
		execlp("ps","ps","-ef",0);
		perror("execlp ps -ef");
	}
	close(pfd[1]);
	close(pfd[0]);
	wait(&state);
	wait(&state);
}

