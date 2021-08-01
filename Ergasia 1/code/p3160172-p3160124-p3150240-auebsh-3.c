#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "p3160172-p3160124-p3150240-auebsh-common.c"

static char line[1024];//variable for input line from user!!
 
int main(int argc, char *argv[])
{
	
	
	printf("AUEB SHELL: To exit press ctrl +D.\n");//print the info of our shell
	while (1) 
	{
		// Print the command prompt 
		printf("auebsh3> ");
		fflush(NULL);
 
		// Read a command line,if ctrl+D is pressed exit is done 
		if (!fgets(line, 1024, stdin)) {
			return 0;
		}
		int input = 0;//input: return value from previous command (useful for pipe file descriptor)
		int first = 1;//first: 1 if first command in pipe-sequence (no input from previous pipe)
		int permition=1;// permition:if  command contains parameters like -l -r etc gives permition to execute them
		int temp = sizeof(line);//size of line
		char* cmd = line;//pointer which shows at the start of lines
		input = run(cmd, input, first, 1,temp,permition);//call run funtion
		wait_pid();//wait for child to end
	}
	return 0;
	
}


