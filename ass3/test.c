#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <stdlib.h>


#define __NR_set_task_params_sys 341
#define __NR_get_task_params_sys 342

int main(void){
	struct d_params *params=(struct d_params *) malloc(sizeof(struct d_params));
	int deadline=3;
	int estimated_runtime=20;

	if(syscall(__NR_set_task_params_sys, 3,20)==0){
		printf("good set\n");
	}

	if(syscall(__NR_get_task_params_sys, params)==0){
		printf("good get\n");
	}

	


	
	



	
	printf("Set did:\nDeadline:%d\nEstimated runtime:%d\n",deadline,estimated_runtime);
	printf("Get did:\nDeadline:%d\nEstimated runtime:%d\n",params->deadline,params->estimated_runtime);
	
	free(params);


	return 0;
}
