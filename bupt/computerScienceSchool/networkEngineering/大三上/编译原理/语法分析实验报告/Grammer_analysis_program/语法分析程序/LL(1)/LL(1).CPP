#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dos.h>
struct Stack{
	char  s[30];
	int top; /*栈顶指针      */
}S1;
int num=0;
char v1[6]={'i','+','*','(',')','#'};/*终结符  */
char v2[5]={'E','G','T','S','F'};/*非终结符   */
/*用二维数组保存预测分析表，可用符号^来代替ε，注意字符串结束位自动加'\0'   */
char table[5][6][4]={
                     {"GT","","","GT","",""},{"","GT+","","","^","^"},
                     {"SF","","","SF","",""},{"","^","SF*","","^","^"},
                     {"i","","",")E(","",""}
                     };

void print()/*输出分析栈  */
{
	int a;/*指针*/
	for(a=0;a<S1.top;a++)
		printf("%c",S1.s[a]);
	printf("\t");
}/*print*/

void intialstack()     
{
	S1.top=0;
}
/*初始化栈*/
void push(char ch)     
{
	S1.s[S1.top]=ch;
	S1.top++;
}
/*压栈*/
char pop( )     
{
	S1.top--;
	return S1.s[S1.top]; 
}
/*出栈*/
void   jinzhan(int hang,int lie)
{ 
	char *chuan;
	chuan=table[hang][lie];
	chuan[3]='\0';
	int i=0;
	while(chuan[i]!=NULL&&chuan[0]!='^')
	{
		push(chuan[i]);
		i++;
	}
}
/*进栈操作，将符号串数组压入栈中*/
int   iszhongjie(char X )
{
	for(int i=0;i<6;i++)
		if(X==v1[i])
			return i;
		return -1;
		
}
/*判断X是否为终结符，返回下标*/
int   isfzhongjie(char X )
{
	for(int i=0;i<5;i++)
		if(X==v2[i])
			return i;
		return -1;
		
}
/*判断X是否为非终结符，返回下标*/
bool  chabiao(char X,char sym)
{
	int hang=isfzhongjie(X);
    int lie=iszhongjie(sym);
	if(hang!=-1&&lie!=-1)
	{
		if(sym!='^')
		{
			printf("%c->",X);
			for(int i=(strlen(table[hang][lie])-1);i>=0;i--)
				printf("%c",table[hang][lie][i]);
			printf("\n");
		}
		jinzhan(hang,lie);
		return true;
	}
	else 
	{
		printf("错误\n");
		return false;
	}
	
}
/*判断X是否为非终结符，sym是否为终结符，若是查找预测表对应表格是否为空白,是则出错，否则进栈*/
void shuchuchuan()
{
	FILE *fp1;
	char c;
	int num1=0;
	if((fp1=fopen("输入文件.txt","r"))==NULL){   //读取文件内容，并返回文件指针，该指针指向文件的第一个字符
		fprintf(stderr,"error opening.\n");
        exit(1);
	}
	c=fgetc(fp1);
	while(c!='#')
	{
		if(num1>=num)
			printf("%c",c);
		else
			printf(" ");
		c=fgetc(fp1);
		num1++;
	}
	printf("#\t");
}
int main()
{ 
	FILE *fp;char sym,X;bool flag,cuo;
	if((fp=fopen("输入文件.txt","r"))==NULL){   //读取文件内容，并返回文件指针，该指针指向文件的第一个字符
		fprintf(stderr,"error opening.\n");
        exit(1);
	}
	intialstack();
	push('#');
	push('E');
	
	sym=fgetc(fp);/*把第一个输入符号读进sym*/
	
	flag=true;
	cuo=false;
	printf("符号栈\t输入串\t所用产生式\n");
	while(flag&&!cuo)
	{
		print();
		shuchuchuan();
		X=pop();
		if (X=='#') 
		{ 
			if(sym==X)
			{
				printf("成功");
				flag=false;
			}
			else
				printf("错误");
		}
		else if(iszhongjie(X)!=-1)
		{ 
			printf("\n");
			if(X==sym)
			{
				num++;
				sym=fgetc(fp);
			}
			else
				printf("错误");
		}
		else if(!chabiao(X,sym))
		{
			cuo=true;
		}
	}
	printf("\n");
	system("pause");
	return 0;
}
