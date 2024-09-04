#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define __NR_set_task_params_sys 341
#define __NR_get_task_params_sys 342

int main(void){
	if(syscall(__NR_set_task_params_sys,3,20000)==0){
		printf("this shouldnt happen");
	}
	if(syscall(__NR_get_task_params_sys,NULL)==0){
		printf("this shouldnt happen either");
	}
	
	printf("good\n");

	return 0;
}
	
