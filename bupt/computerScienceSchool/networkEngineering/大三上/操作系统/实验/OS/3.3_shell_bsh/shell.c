#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BUFFERSIZE 256
//最简单的shell，只是简单的执行命令调用，没有任何的其他功能
int main()
{
	char buf[BUFFERSIZE],*cmd,*argv[100];
	char inchar;
	int n,sv,buflength;
	int result;
	buflength = 0;
	for(;;) {
		printf("=> ");
		//处理过长的命令；
		inchar = getchar();//读取命令
		while (inchar != '\n' && buflength < BUFFERSIZE ){
			buf[buflength++] = inchar;
			inchar = getchar();
		}
		if (buflength > BUFFERSIZE){
			printf("Command too long,please enter again!\n");
			buflength = 0;
			continue;
		}
		else
		buf[buflength] = '\0';
		//解析命令行，分成一个个的标记
		//char *strtok(char *s,char *delim)
		//分解字符串为一组字符串。s为要分解的字符串，delim为分隔符字符串。
		cmd=strtok(buf," \t\n");
		if(cmd) {
			if(strcmp(cmd,"exit")==0) exit(0);
			n=0;
			argv[n++]=cmd;
			while(argv[n++]=strtok(NULL," \t\n"));
			if(fork()==0) {
				execvp(cmd,argv);
				fprintf(stderr,"sxh:%s:command not found.\n",buf);//如果子进程顺利执行，这段话是不会执行的
				exit(1);
			}
			wait(&sv);
			buflength = 0;
		}
	}
} 