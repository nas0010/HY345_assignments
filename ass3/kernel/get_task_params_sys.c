#include <linux/kernel.h>
#include <asm-generic/errno-base.h>
#include <asm/current.h>
#include <asm/uaccess.h>
#include <linux/sched.h>

asmlinkage long sys_get_task_params_sys(struct d_params *params){
	struct d_params current_params;
	struct task_struct *current_process;
	current_process=get_current();
	current_params.deadline=current_process->deadline;
	current_params.estimated_runtime=current_process->estimated_runtime;
	printk("Nasia Boubouraki, csd4692, sys_get_task_params_sys was called.\n");
	if(params!=NULL && (access_ok(VERIFY_WRITE,params,sizeof(struct d_params))!=0)){			
		if(copy_to_user(params,&current_params,sizeof(struct d_params))==0){
			return 0;
		}else return EINVAL;
	}else{
		return EINVAL;
	}

	return 0;
}
