#include<stdio.h> 
#include<stdlib.h> 

int main() 
{ 
	int i,p; 
	p = fork(); 
	if(p > 0)	exit(0);  
	else if(p == 0)
	{ 
		for(i = 0; i < 10; i++)	
		{	
			sleep(10); 
			system("ps > f1.txt");
		} 
	} 
	else
		perror("Create new process!");  
	return 1;
}
