/* the Tqueue driver  for Lab 1*/
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/time.h>
#include "T_Queue.h"

#define DEVICE_NAME "cmos"
#define NUM_TQUEUE_BANKS 2
#define MAX_DATA_BYTES 96

/* per device structure */
struct Tqueue_dev {
	struct cdev cdev;               /* The cdev structure */
	char name[10];                  /* Name of I/O region */
	T_Q queue;			/* queue object */
	spinlock_t TQ_lock;		/* spinlock object */
} *cmos_devp[NUM_TQUEUE_BANKS];


static dev_t queue_dev_number;      /* Allotted device number */
struct class *queue_class;          /* Tie with the device model */


/* Driver Exit */
void __exit Tqueue_exit(void)
{
	int i;

	/* Release the major number */
	unregister_chrdev_region((queue_dev_number), NUM_TQUEUE_BANKS);

	/* Destroy device */
	for (i=0; i<NUM_TQUEUE_BANKS; i++) {
			device_destroy (queue_class, MKDEV(MAJOR(queue_dev_number), i));
			cdev_del(&cmos_devp[i]->cdev);
			kfree(cmos_devp[i]);
	}

	/* Destroy queue_class */
	class_destroy(queue_class);
}

/*
* Open QUEUE
*/
int queue_open(struct inode *inode, struct file *file)
{
	struct Tqueue_dev *cmos_devp;

	/* Get the per-device structure that contains this cdev */
	cmos_devp = container_of(inode->i_cdev, struct Tqueue_dev, cdev);

	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = cmos_devp;

	printk("\n%s is openning\n", cmos_devp->name);
	return 0;
}

/*
 * Release QUEUE
 */
int queue_release(struct inode *inode, struct file *file)
{
	struct Tqueue_dev *cmos_devp = file->private_data;
	
	printk("\n%s is closing\n", cmos_devp->name);
	
	return 0;
}

/*
 * Read from a queue
 */
ssize_t queue_read(struct file *file, char *buf,
          size_t count, loff_t *ppos)
{
	int res = 0;
	int len = 0;
	unsigned long flags;
	unsigned char d_ts[3];

	struct Tqueue_dev *cmos_devp = file->private_data;	
	char* dq_data = kmalloc(MAX_DATA_BYTES, GFP_KERNEL);

	spin_lock_irqsave(&(cmos_devp->TQ_lock), flags);		//lock the dequeue operation 
	res = T_dequeue(cmos_devp->queue, dq_data);
	spin_unlock_irqrestore(&(cmos_devp->TQ_lock), flags);		//unock the dequeue operation

	if (res < 0)							//if queue was empty return -1
		return -1;
	
	//get departure time stamp	
	d_ts[0] = rtc_cmos_read(4);					//read hour
	d_ts[1] = rtc_cmos_read(2);					//read minutes
	d_ts[2] = rtc_cmos_read(0);					//read seconds
	//got timestamp in place of this
	
	//pack the data into a buffer to send to user	
	len = res;
	memcpy(dq_data+len, d_ts, sizeof(d_ts));
	
	printk("\n In queue_read, dequeued packet length:%d, data:%d", len, *(int*)dq_data);
	
	len+=sizeof(d_ts);
	memcpy(dq_data, &len, sizeof(len));
	printk("\n final data:%d",*(int*)dq_data);
	//After packing data
	
	res = copy_to_user((void __user *)buf, (void *)dq_data, len);
	
	if(res>0) 
		len = 0;		// if some data was not written return 0
	else
		len-=res;		//else return the length that was written
	
	kfree(dq_data);	
	return len;
}

/*
 * Write to a queue
 */
ssize_t queue_write(struct file *file, const char *buf,
           size_t count, loff_t *ppos)
{
	int res, index=0;	
	unsigned char a_ts[3];	
	unsigned long flags;	
	struct Tqueue_dev *cmos_devp = file->private_data;
	
	char *temp_buf = kmalloc(count, GFP_KERNEL);
	int len = count + sizeof(int) + sizeof(a_ts);	
	char *kbuf = kmalloc(len, GFP_KERNEL);	
		
	res = copy_from_user((void *)temp_buf, (void __user *)buf, count);						
	
	//get the time_stamp
	a_ts[0] = rtc_cmos_read(4);		//read hour
	a_ts[1] = rtc_cmos_read(2);		//read minute
	a_ts[2] = rtc_cmos_read(0);		//read sec
	//got the timestamp

	//Pack the timestamp	
	memcpy(kbuf, &len, sizeof(len));
	index+=sizeof(len);
	
	memcpy(kbuf+index, temp_buf, count);
	index+=count;
	
	memcpy(kbuf+index, a_ts, sizeof(a_ts));
	//After packing timestamp	
		
	//obtain lock and enqueue packet 	
	spin_lock_irqsave(&(cmos_devp->TQ_lock), flags);	//lock the enqueue operation
	res = T_enqueue(kbuf, cmos_devp->queue);
	spin_unlock_irqrestore(&(cmos_devp->TQ_lock), flags);
	//Afer enqueue release lock				 //unlock the enqueue operation 

	if (res < 0)					 //if queue was full return -1 */
		return -1;
	
	kfree(temp_buf);
	kfree(kbuf);
	return count;
}


/* File operations structure. Defined in linux/fs.h */
static struct file_operations cmos_fops = {
    .owner=THIS_MODULE,           /* Owner */
    .open=queue_open,              /* Open method */
    .release=queue_release,        /* Release method */
    .read=queue_read,              /* Read method */
    .write=queue_write,            /* Write method */
};

/*
 * Driver Initialization
 */
int __init Tqueue_init(void)
{
	int i, ret;

	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&queue_dev_number, 0, NUM_TQUEUE_BANKS, DEVICE_NAME) < 0) {
			printk(KERN_DEBUG "Can't register device\n"); return -1;
	}

	/* Populate sysfs entries */
	queue_class = class_create(THIS_MODULE, DEVICE_NAME);

	for (i=0; i<NUM_TQUEUE_BANKS; i++) {
			
		/* Allocate memory for the per-device structure */
		cmos_devp[i] = kmalloc(sizeof(struct Tqueue_dev), GFP_KERNEL);
		
		if (!cmos_devp[i]) {
				printk("Bad Kmalloc\n"); return -ENOMEM;
		}

		/* Request I/O region */
		sprintf(cmos_devp[i]->name, "Tqueue%d", i+1);


		/* Connect the file operations with the cdev */
		cdev_init(&cmos_devp[i]->cdev, &cmos_fops);
		cmos_devp[i]->cdev.owner = THIS_MODULE;

		/* init the spinlock */		
		spin_lock_init(&(cmos_devp[i]->TQ_lock));

		/* init the queue */
		cmos_devp[i]->queue = initialize_TQueue();

		/* Connect the major/minor number to the cdev */
		// Not sure what (queue_dev_number +i) means
		ret = cdev_add(&cmos_devp[i]->cdev, (queue_dev_number + i), 1);

		if (ret) {
			printk("Bad cdev\n");
			return ret;
		}

		/* Send uevents to udev, so it'll create /dev nodes */
		device_create(queue_class, NULL, MKDEV(MAJOR(queue_dev_number), i), NULL, "Tqueue%d", i+1);		
	}

	printk("Tqueue Driver Initialized.\n");
	return 0;
}

module_init(Tqueue_init);
module_exit(Tqueue_exit);
MODULE_LICENSE("GPL v2");
