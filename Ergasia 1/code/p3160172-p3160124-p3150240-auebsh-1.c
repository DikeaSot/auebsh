#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "p3160172-p3160124-p3150240-auebsh-common.c"
static char line[1024];//variable for input line from user!!
 
int main()
{
	printf("AUEB SHELL: Send press ctrl +D.\n");//print the info of our shell
	while (1) {
		// Print the command prompt 
		printf("auebsh1> ");
		fflush(NULL);
 
		// Read a command line 
		if (!fgets(line, 1024, stdin)) 
			return 0;
			
		int input = 0;//input: return value from previous command (useful for pipe file descriptor)
		int first = 1;//first: 1 if first command in pipe-sequence (no input from previous pipe)
 		int permition=0;// permition:if  command contains parameters like -l -r etc print execution problem!
		int temp = sizeof(line);//size of line
		char* cmd = line;//pointer which shows at the start of lineS
		input = run(cmd, input, first, 1,temp,permition);//call run funtion
		wait_pid();//wait for child to end
		
	}
	return 0;
}
 

