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
void execPipe(char **cmdp, int count);
int separatePipe(char *input, char **read);
//execute piped commands
void execPipe(char **input, int count){
	int k,i,status;
	pid_t p;
	int fd[count][2];
	for(k=0;k<count;k++){
		if(pipe(fd[k])==-1){ printf("oops pipe"); return;}
	}
	for(i=0;i<count;){
		p=fork();
		if(p<0) return;
		
	        if(p==0){
			char *commands[35];
			separateLine(input[i],commands);
			//first pipe
			if(i<count-1){
				dup2(fd[i][1],1);
			}
			if(i!=0){
				dup2(fd[i-1][0],0);	
			}
			for(k=0;k<count;k++){
				close(fd[k][0]);
				close(fd[k][1]);
			}
			if(execvp(commands[0],commands)<0){
				printf("kakh entolh");
				exit(EXIT_FAILURE);
			}else printf("eimia edw");
			
		}
	
	i++;
	}


	for(k=0;k<count;k++){
		close(fd[k][0]);
		close(fd[k][1]);
	}
	for(k=0;k<count;k++){
		wait(&status);
	}
	
}


void signal_callback_handler(int signum){
	printf("Caught signal %d\n",signum);
	exit(signum);
}

//get,set,export variable
void setVariable(char*command){
	char*tmp, * var,*value;
	var=strsep(&command,"=");
	value=strsep(&command,"=");
	if(value[0]=='"'){
		tmp=value+1;
		tmp[strlen(value)-2]='\0';
	}else tmp=value;
	setenv(var,tmp,1);	
}

//get input command
int getInput(char* command){
	char* temp;
	temp=readline("\n>>");
	if(strlen(temp)==0){
		return -1;
	}else{
		strcpy(command,temp);
		free(temp);
		return 0;
	}
}

//separate input to cammand and arguments
void separateLine(char *input, char** read){
	int i=0;
	char* tmp;
	
	tmp=strtok(input," ");
	while(tmp!=NULL){
		read[i]=tmp;
		i++;
		tmp=strtok(NULL," ");
	}
	read[i]=NULL;
}
		 
//separate input to pipes
int separatePipe(char *input,char** read){
	int i=0,counter=0;
	char* tmp;
	
	tmp=strtok(input,"|");
	while(tmp!=NULL){
		counter++;
		read[i]=tmp;
		i++;
		tmp=strtok(NULL,"|");
	}
	read[i]=NULL;
	
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
	}else{
		printf("Fork not executed successfuly:\n");
	}


}
//cd and exit execution
void execSpecial(char** command){
	int i=0;
	if(strcmp(command[0],"cd")==0){
		chdir(command[1]);
		return;
	}else if(strcmp(command[0],"exit")==0){
		exit(0);
	}else if(strcmp(command[0],"echo")==0 && command[1][0]=='$'){
		char tmp[100];
		int i=0;
		for(i=0;i<strlen(command[1]);i++){
			tmp[i]=command[1][i+1];//+1to remove $
		}tmp[i-1]='\0';
		printf("%s\n",getenv(tmp));
	}else execBasic(command);

} 
//wanted format for user and directory info
void printUser(){
	char cdir[100];
	char user[50];
	getlogin_r(user,sizeof(user));
	getcwd(cdir,sizeof(cdir));
	printf("%s@cs345sh/%s$",user,cdir);
}


int main(){
	char input[100],*separated[100];
	int count;
	while(1){
		printUser();
		if(getInput(input)) continue;
		if(strchr(input,'=')!=NULL && strchr(input,'|')==NULL){
			setVariable(input);
		}else if(strchr(input,'|')!=NULL){
			count=separatePipe(input,separated);
			execPipe(separated,count);
		}else{
			separateLine(input,separated);
			execSpecial(separated);		
		}signal(SIGINT, signal_callback_handler);
	//	signal(SIGTSTP, signal2_callback_handler);

	
	}


	return 0;
}
