/**
 *  simple-procfs-kmod.c -  Simple kmod that creates a "file" in /proc
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#define BUFSIZE  100

#define PROCFS_NAME "simple-procfs-kmod"

// Set module version 
MODULE_VERSION(KMODVER);

// Code taken from https://github.com/dev-area/proctest
// and modified slightly to use the `simple-procfs-kmod` file
// under the proc filesystem.
//
// Original attribution to author here:
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Liran B.H");

static int number=0;
module_param(number,int,0660);

static struct proc_dir_entry *ent;

static ssize_t mywrite(struct file *file, const char __user *ubuf,size_t count, loff_t *ppos) 
{
    int num,c,i;
    char buf[BUFSIZE];
    if(*ppos > 0 || count > BUFSIZE)
        return -EFAULT;
    if(raw_copy_from_user(buf,ubuf,count))
        return -EFAULT;
    num = sscanf(buf,"%d",&i);
    if(num != 1)
        return -EFAULT;
    number = i;
    c = strlen(buf);
    *ppos = c;
    return c;
}

static ssize_t myread(struct file *file, char __user *ubuf,size_t count, loff_t *ppos) 
{
    char buf[BUFSIZE];
    int len=0;
    if(*ppos > 0 || count < BUFSIZE)
        return 0;
    len += sprintf(buf,"%s number = %d\n",PROCFS_NAME, number);
    
    if(raw_copy_to_user(ubuf,buf,len))
        return -EFAULT;
    *ppos = len;
    return len;
}

// the api for procfs changed with the 5.6 kernel to use the proc_ops struct rather than file_operations
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static loff_t proc_lseek(struct file *file, loff_t offset, int i)
{
    file->f_pos = offset;
    return offset;
}
static struct proc_ops myops =
{
    .proc_read = myread,
    .proc_write = mywrite,
    .proc_lseek = proc_lseek,
};
#else
static struct file_operations myops = 
{
    .owner = THIS_MODULE,
    .read = myread,
    .write = mywrite,
};
#endif

static int simple_init(void)
{
    ent=proc_create(PROCFS_NAME,0660,NULL,&myops);
    printk(KERN_INFO "%s: hello ...\n", PROCFS_NAME);
    return 0;
}

static void simple_cleanup(void)
{
    proc_remove(ent);
    printk(KERN_INFO "%s: bye ...\n", PROCFS_NAME);
}

module_init(simple_init);
module_exit(simple_cleanup);
