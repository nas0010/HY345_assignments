#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

//declarations
void printUser();
void execSpecial(char** commands);
int getInput(char *command);
void separateLine(char* input, char** read);
void setVariable(char* command);
void execBasic(char **cmd);
void execPipes(char *cmdp,int count);
int separatePipe(char *input, char **read);
void manipulatePipe(int fd[][2],int count,char* command);

//close all pipes
void manipulatePipe(int fd[][2],int count,char* command){
	int i;
	if(command=="create"){
		for(i=0;i<count;i++){
			if(pipe(fd[i])==-1) return;
		}
	}else if(command=="close"){
		for(i=0;i<count;i++){
			close(fd[i][0]);
			close(fd[i][1]);
		}
		return;
	}	
}

//execute piped commands
void execPipe(char **input, int count){
	int i,status;
	pid_t p;
	int fd[count][2];//file descriptors as many as the pipes 0:read 1:write
	manipulatePipe(fd,count,"create");
	
	for(i=0;i<count;){
		p=fork();
		if(p<0) return;
		if(p==0){
			char *commands[35];
			separateLine(input[i],commands);
			 //not last,meaning the output of current pipe will be used from next pipe
			if(i<count-1)	dup2(fd[i][1],1);
	
		
			//not first,meaning that i will read from previous pipe
			if(i>0)	dup2(fd[i-1][0],0);	
			
			manipulatePipe(fd,count,"close"); //close all pipes since i dup2 
			
			execvp(commands[0],commands); //execute commands	
			
		}
		i++;
	}
	//close pipes and wait for children
	manipulatePipe(fd,count,"close");
	
	for(i=0;i<count;i++) wait(&status);
	
}

//given code for ctrl C
void signal_callback_handler(int signum){
	printf("Caught signal %d\n",signum);
	exit(signum);
}

//get,set,export variable
void setVariable(char*command){
	char*tmp, * var,*value;
	var=strsep(&command,"=");
	value=strsep(&command,"=");
	if(value[0]=='"'){ //remove "" when setting variable
		tmp=value+1;
		tmp[strlen(value)-2]='\0';
	}else tmp=value;
	setenv(var,tmp,1);	
}

//get input command
int getInput(char* command){
	char* temp;
	temp=readline("\nGive me commands:) >>");
	if(strlen(temp)==0){
		return -1;
	}else{
		strcpy(command,temp);
		return 0;
	}
}

//separate input to cammand and arguments
void separateLine(char *input, char** read){
	int i=0;
	char* tmp;
	
	tmp=strtok(input," ");
	for(i=0;tmp!=NULL;i++){
		read[i]=tmp; //put in each row of read[][] a piece from command 
		tmp=strtok(NULL," ");
	}
	read[i]=NULL;//\0 terminated
}
		 
//separate input to pipes
int separatePipe(char *input,char** read){
	int i=0,counter=0;
	char* tmp;
	
	tmp=strtok(input,"|");
	for(i=0;tmp!=NULL;i++){
		counter++; //count number of pipes
		read[i]=tmp;//put in each row of read[][] a pipe 
		tmp=strtok(NULL,"|");
	}
	read[i]=NULL;//\0 terminated
	
	return counter;
}
	


//execute basic command
void execBasic(char** command){
	int status;
	int pid=fork();
	if(pid==0){
		execvp(command[0],command);
	}else if(pid>0){
		waitpid(pid,&status,0);
	}else return;
}

//cd and exit execution and control of all commands
void execSpecial(char** command){
	int i=0;
	if(strcmp(command[0],"cd")==0){
		chdir(command[1]); //in command[1] find path
		return;
	}else if(strcmp(command[0],"exit")==0){
		exit(0);
	}else if(strcmp(command[0],"echo")==0 && command[1][0]=='$'){
		char tmp[50]; //name of variable
		for(i=0;i<strlen(command[1]);i++){
			tmp[i]=command[1][i+1];//+1to remove $
		}
		tmp[i-1]='\0';
		printf("%s\n",getenv(tmp));
	}else execBasic(command);

}
 
//wanted format for user and directory info
void printUser(){
	char cdir[100],user[50];
	getlogin_r(user,sizeof(user));//name of user
	getcwd(cdir,sizeof(cdir));//directory
	printf("%s@cs345sh/%s$",user,cdir);
}


int main(){
	char input[420],*separated[100]; //input of user up to 420;) chars and 100 pipes (or command and attributes)
	int count; //for num of pipes
	while(1){
		printUser();
		if(getInput(input)) continue; //make sure the user gave sth
		if(strchr(input,'=')!=NULL && strchr(input,'|')==NULL){
			setVariable(input);//not a pipe and i hav to set variable
		}else if(strchr(input,'|')!=NULL){//pipes
			count=separatePipe(input,separated);
			execPipe(separated,count); //get count and execute pipes
		}else{//simple command
			separateLine(input,separated);
			execSpecial(separated);		
		}signal(SIGINT, signal_callback_handler);// ctrl c
	//	signal(SIGTSTP, signal2_callback_handler); //ctrl z

	
	}

	free(input);
	free(separated);
	return 0;
}
