#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

//MODULE_LICENSE("Dual BSD/GPL");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("clock module for gettimeofday by proc.");
MODULE_AUTHOR("Flameshark");

//#define MODULE
#define MODULE_VERSION1 "Linux bt 3.2.6 #1 SMP Fri Feb 17 10:40:05 EST 2012 i686 GNU/Linux"
#define MODULE_NAME1 "clock"

int proc_read_clock(char* page, char** start, off_t off, int count,int* eof,void* data)
{
    	int len;
    	struct timeval xtime;
    	do_gettimeofday(&xtime);	//Get the time
    	len = sprintf(page,"%d|%d\n",xtime.tv_sec,xtime.tv_usec);
    	printk("clock: read_func()\n");
    	return len;	//Print the time
}

static int my_module_init(void)
{
    	struct proc_dir_entry * my_clock;
    	printk("clock: init_module()\n");
	//my_clock=create_proc_read_entry("clock",0,&proc_root, proc_read_clock,0);
    	my_clock=create_proc_read_entry("clock",0,NULL, proc_read_clock,0);	//Creat the entry
    	printk(KERN_INFO "%s %s has initialized.\n",MODULE_NAME1,MODULE_VERSION1);
    	return 0;
}

static void my_module_exit(void)
{
    	printk("clock: cleanup_module()\n");
    	//remove_proc_entry(proc_my_clock->name, &proc_root);
    	//remove_proc_entry(proc_my_clock->name, NULL);
    	remove_proc_entry("clock",NULL);	//Remove the entry
    	printk(KERN_INFO "%s %s has removed.\n",MODULE_NAME1,MODULE_VERSION1);
} 
module_init(my_module_init);
module_exit(my_module_exit);

EXPORT_NO_SYMBOLS;
