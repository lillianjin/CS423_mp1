#define LINUX

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include "mp1_given.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group_ID");
MODULE_DESCRIPTION("CS-423 MP1");

#define DEBUG 1
#define DIRECTORY "mp1"
#define FILENAME "status"

// Declare proc filesystem entry
static struct proc_dir_entry *proc_dir, *proc_entry;
// Declare message buffer
char *msg;
// Declare file buffer
struct mutex lock;

// Create a list head, which points to the head of the linked list
LIST_HEAD(list_head);
// Define a linked list proc_list
typedef struct proc_node {
    struct list_head head;
    int pid;
    unsigned long cpu_time;
} proc_node;


/*
Read from User
filp: file pointer
buf: points to the user buffer holding the data to be written or the empty buffer
count: the size of the requested data transfer
offp: a pointer to a “long offset type” object that indicates the file position the user is accessing
*/
ssize_t mp1_read (struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
    int copied = 0;
    proc_node *curr;

    // if the file is read
    if(*offp > 0){
        return 0;
    }

    //allocate memory in buffer using kmalloc(size_t size, gfp_t flags);
    //size: bytes of memory required; flags: the type of memory to allocate.
    char *buffer = (char *)kmalloc(4096, GFP_KERNEL);

    // Acquire the mutex
    mutex_lock(&lock);

    /*
    Read the whole list
    list_for_each_entry (pos, head, member);
    pos: the type * to use as a loop cursor.
    head: the head for your list.
    member: the name of the list_head within the struct.
    */
    char *str_buff = (char*)kmalloc(512, GFP_KERNEL);
    list_for_each_entry(curr, &list_head, head){
        int cur_len = sprintf(str_buff, "%lu: %lu\n", curr->pid, curr->cpu_time);
        copied += cur_len;
        kfree(str_buff);
        strcpy(buffer + cur_len, str_buff);
    };

    mutex_unlock(&lock);

    // if the message length is larger than buffer size
    if (copied > 4096){
        copied = 4096;
    }

    //copy_to_user — Copy a block of data into user space.
    //copy_to_user(void __user *to, const void *from, unsigned long n);
    //to: Destination address in user space. from: Source address in kernel space. n: Number of bytes to copy.
    copy_to_user(buf, buffer, copied);

    // '\0' - End of string character with value 0 (NUL)
    buffer[copied + 1] = '\0';

    // kfree — free previously allocated memory
    kfree(buffer);

    return *offp = copied;
}


/*
Write to User
*/
ssize_t mp1_write (struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
    // Fetch PID from user
    long mypid;
    char *buffer = (char*)kmalloc(count, GFP_KERNEL);
    copy_from_user(buffer, buf, count);
    buffer[count] = '\0';
    // change string to integer
    kstrtol(buffer, 10 , &mypid);
    // Create a new node
    proc_node *new = (proc_node *)kmalloc(sizeof(proc_node), GFP_KERNEL);
    new -> pid = mypid;
    unsigned long cpu;

    mutex_lock(&lock);
    if(get_cpu_use(new->pid, &cpu)==0){
        new -> cpu_time = cpu;
        INIT_LIST_HEAD(&(new->head));
        // Add the node to the linked list
        list_add(&new->head, &list_head);
    } else {
        return -ENOMEM;
    }

    mutex_unlock(&lock);
    kfree(buffer);
    kfree(new);
    return count;
}

// Declare the file operations
static const struct file_operations file_fops = {
    .owner = THIS_MODULE,
    .write = mp1_write,
    .read  = mp1_read,
};



// mp1_init - Called when module is loaded
int __init mp1_init(void)
{
   #ifdef DEBUG
   printk(KERN_ALERT "MP1 MODULE LOADING\n");
   #endif
   // Insert your code here ...

   /*
   Create proc directory "/proc/mp1/" using proc_dir(dir, parent)
   dir: The name of the folder that will be created under /proc.
   parent: In case the folder needs to be created in a sub folder under /proc a pointer to the same is passed else it can be left as NULL.
   */
   proc_dir = proc_mkdir(DIRECTORY, NULL);

   /*
   Create file entry "/proc/mp1/status/" using proc_create(name, mode, parent, proc_fops)
   name: The name of the proc entry
   mode: The access mode for proc entry
   parent: The name of the parent directory under /proc
   proc_fops: The structure in which the file operations for the proc entry will be created.
   */
   proc_entry = proc_create(FILENAME, 0666, proc_dir, &file_fops);

   printk(KERN_ALERT "MP1 MODULE LOADED\n");
   return 0;
}

// mp1_exit - Called when module is unloaded
void __exit mp1_exit(void)
{
   #ifdef DEBUG
   printk(KERN_ALERT "MP1 MODULE UNLOADING\n");
   #endif
   // Insert your code here ...

   /*
   remove /proc/mp1/status and /proc/mp1 using remove_proc_entry(*name, *parent)
   Removes the entry name in the directory parent from the procfs
   */
   remove_proc_entry(FILENAME, proc_dir);
   remove_proc_entry(DIRECTORY, NULL);


   printk(KERN_ALERT "MP1 MODULE UNLOADED\n");
}

// Register init and exit funtions
module_init(mp1_init);
module_exit(mp1_exit);
