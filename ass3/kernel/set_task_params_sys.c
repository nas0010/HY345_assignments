#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm-generic/errno-base.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

asmlinkage long sys_set_task_params_sys(int deadline,int estimated_runtime){
	struct task_struct *current_process;
	current_process=get_current();
	printk("Nasia Boubouraki ,csd4692, sys_set_task_params_sys was called.\n");
	if((estimated_runtime<=1000*deadline) && estimated_runtime>0){
		current_process->deadline = deadline;
		current_process->estimated_runtime = estimated_runtime;
				return 0;

	}else{
		return EINVAL;
	}	
}
