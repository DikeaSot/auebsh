#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "p3160172-p3160124-p3150240-auebsh.h"

static char* args[512];//pointer table of char 
pid_t pid;//variable pid_t type
int command_pipe[2];// table int variable  
static int n = 0; /* number of calls to 'command' */
#define READ  0
#define WRITE 1
 int command(int input, int first, int last,char rfile[],char wfile[])
{
	int pipettes[2]; //variable for pipe

	// Invoke pipe
	pipe( pipettes );	// pipe() is used for passing information from one process to another
	pid = fork();		//Fork system call use for creates a new process, which is called child process, which runs concurrently with process and this process is called parent process
	if (pid == 0) {//if no error or 'zombie'programm, from child exists 
		if (first == 1 && last == 0 && input == 0) {
			// First command of piped commands
			
			if(rfile != "" && strchr(rfile, '.') != NULL){//if input redirection file exists .....
				
				int textin = open(rfile, O_RDONLY);//open it
		
				if(textin == STDERR_FILENO) {//if any error due opening ....print error
					printf("Something went wrong! File cannot be opened!\n");
				}else{
					dup2(textin,0);
					printf("File successfully opened!\n");
				}
			}
			dup2( pipettes[WRITE], STDOUT_FILENO );//print result in terminal
		} else if (first == 0 && last == 0 && input != 0) {
			// Middle command and no redirections exists...default input and write in terminal
			dup2(input, STDIN_FILENO);
			dup2(pipettes[WRITE], STDOUT_FILENO);
		}else if(first == 1 && last == 1 && input == 0){//if is only one command ,of redirection (input and output)...... 
			 if(rfile != "" && strchr(rfile, '.') != NULL){
				
				int textin = open(rfile, O_RDONLY);//open to read
		
				if(textin == STDERR_FILENO) {//if error opening print error
					printf("Something went wrong! File cannot be opened!\n");
				}else{// change default input to the given one
					dup2(textin,0);
					printf("File successfully opened!\n");
				}
			}
			if(wfile != "" && strchr(wfile, '.') != NULL){
				
				int textout = open(wfile,O_WRONLY|O_CREAT|O_TRUNC,S_IWUSR|S_IRUSR); //open to write or creat new if doesnt exist..

				if(textout == STDERR_FILENO){//if error opening print error
					printf("Something went wrong! File cannot be written!\n");
				}else{// change default output to the given one
					printf("File successfully written!\n");
					dup2(textout,1);
				}
			}	
		}else {
			// Last command in piped commands ...
				dup2( input, STDIN_FILENO );//take input
			if(wfile != "" && strchr(wfile, '.') != NULL){
				
				int textout = open(wfile,O_WRONLY|O_CREAT|O_TRUNC,S_IWUSR|S_IRUSR);//open to write or creat new if doesnt exist..

				if(textout == STDERR_FILENO){//if error opening print error
					printf("Something went wrong! File cannot be written!\n");
				}else{// change default output to the given one
					printf("File successfully written!\n");
					dup2(textout,1);
				}	
			}
		}
	
			
		if (execvp( args[0], args) == -1)//execute command
			_exit(EXIT_FAILURE); // If child fails
		
	 }
	if (input != 0) //if any previous command is exists
		close(input);
 
	// Nothing more needs to be written
	close(pipettes[WRITE]);
 
	// If it's the last command, nothing more needs to be read
	if (last == 1)
		close(pipettes[READ]);
 
	return pipettes[READ];
}
 
// Final cleanup, 'wait' for processes to terminate.
 void wait_pid()//function thats does the delay until child ends
{
	pid_t waitPid;
	int status;
	while ((waitPid = wait(&status))>0);
	
}
 
 int run(char* cmd, int input, int first, int last,int szline,int per)
{
	char wfile[20]="";
	char rfile[20]="";

	if (strchr(cmd, '|') != NULL) {
		char* next = strchr(cmd, '|'); // Find first '|' 
		while (next != NULL) {
			//'next' points to '|' 
			*next = '\0';//make next null
			input = run(cmd, input, first, 0, szline, per);
			cmd = next + 1;
			next = strchr(cmd, '|'); // Find next '|' 
			first = 0;
		}
	}
	
	if(strstr(cmd, ">>")!=NULL){// Find if this symbol  '>>' exists 
			printf("Syntax Error!!!\n");
		
	}else if(strchr(cmd, '<') != NULL || strchr(cmd, '>') != NULL){//if exists redirection of input or output file ...
			int flag=0;//variable like counter
			int i;//counter
			int r=0;//counter
			int w=0;//counter
			for(i=0; i<(szline-1); i++){
				
				if(cmd[i] == '<'){
					cmd[i]=' ';
					flag=1; //finds first '<' symbol and changes flag counter value
				}
				if(cmd[i] == '>'){//finds first '>' symbol and changes flag counter value
					cmd[i]=' ';
					flag=2;
				}
				if(flag==1){
					if(cmd[i] != '<' && cmd[i] != ' '){//with correct parameters ,copies the name of user's input file 
						rfile[r]=cmd[i];
						r++;
						cmd[i]=' ';
					}
				}
				if(flag==2){
					if(cmd[i] != '>' && cmd[i] != ' ' && cmd[i] != '\0'){
						wfile[w]=cmd[i];//with correct parameters ,copies the name of user's output file
						w++;
						cmd[i]=' ';
					}
				}
			}//end for
			if(rfile!=""){rfile[r]='\0';}
			if(wfile!=""){wfile[w]='\0';}
			if(per==0){//if per=0 dont give permition to execute the commands like sort -r ls-l.....
				args[1]=NULL;
				split(cmd);
				if(args[1]!=NULL){
					printf("your command is not executable from this shell\n");
					exit(1);
				}
			}else if(per==1){//if per=1  give permition to execute the all types of commands
				split(cmd);

			}			
			if (args[0] != NULL) {//if args[0] which contains the command is not empty execute
				n += 1;
				return command(input, first, last,rfile,wfile);//execute command function
			}
	}else{//if there is no redirection of input or output file .....
		if(per==0){//if per=0 dont give permition to execute the commands like sort -r ls-l.....
			args[1]=NULL;
			split(cmd);
			if(args[1]!=NULL){
				printf("your command is not executable from this shell\n");
				exit(1);
				}
		}else if(per==1){//if per=1  give permition to execute the all types of commands
			split(cmd);
		}	
		if (args[0] != NULL) {//if args[0] which contains the command is not empty execute
			n += 1;
			return command(input, first, last,rfile,wfile);//execute command function
		}
		return 0;
	}
}
  //ignores spaces until finds a char
char* skipspaces(char* s)
{
	while (isspace(*s)) ++s;
	return s;
}
 //splits a string in token whenever a space exists
void split(char* cmd)
{
	cmd = skipspaces(cmd);//trimm spaces
	char* next = strchr(cmd, ' ');//next =the position of cmd space exists
	int i = 0;//counter
 		
	while(next != NULL) {
		next[0] = '\0';//next =null
		args[i] = cmd;//take tokens
		++i;//counter raise
		cmd = skipspaces(next + 1);//next token
		next = strchr(cmd, ' ');//give next the next position of the next space
	}
 
	if (cmd[0] != '\0') {//if command exist
		args[i] = cmd;//take token
		next = strchr(cmd, '\n');
		next[0] = '\0';
		++i; 
	}

	args[i] = NULL;  //pipeline
}
