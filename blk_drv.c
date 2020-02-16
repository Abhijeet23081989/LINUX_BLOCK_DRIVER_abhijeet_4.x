#include<linux/blkdev.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/genhd.h>
#include<linux/fs.h>

#define MY_BLOCK_MAJOR 0 //default block number should be zero so that the kernal can allot the major number
#define MY_BLK_MNR 1
#define My_BLKDEV_NAME "BLOCKADE"
#define NR_SECTORS 1024//?


//++++++++++++Block device structure to store important elements describing the device+++++++++

static struct my_blk_dv
{
	spinlock_t lockdown;//spin-lock is applied for mutual exclusion (So that the block device driver can be accesssed by one user at a time)
	struct gendisk *gd;//struct gendisk is the basic structure in working with block devices
	struct request_queue *que;//The device request queue
}dev;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++Struct Block Device Operations++++++++++++++++++++++++++++++++++++++++++

/*open() and release() operations are called directly from user space by utilities that may 
 *perform the following tasks: partitioning, file system creation, file system verification. 
 *In a mount() operation, the open() function is called directly from the kernel space, the 
 *file descriptor being stored by the kernel. A driver for a block device can not differentiate 
 *between open() calls performed from user space and kernel space.*/
struct block_device_operations{ 
	.owner=THIS_MODULE,
	.open=open_blkdev,//open function is called from the user-space
	.release=release_blkdev,//release function is called from the user-space
}my_blk_fops;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static int open_blkdev(struct block_device *bdev, fmode_t mode)
{
	printk(KERN_INFO"Inside the open_blkdev()\n");
	return 0;
}

void release_blkdev(struct gendisk *gd, fmode_t mode)
{
	printk(KERN_INFO"Inside the release_blkdev()\n");
}
/*Please notice that there are no read or write operations. These operations are performed by the request() function associated with the request queue of the disk.*/
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int status;//status is to hold Major number and also the failed status of register_blkdev()
int count;//everytime open operation is called count goes +1 ,count goes -1 for every user (who called open operation) removed and when count =0 del_gendisk() is called

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static int create_block_device(struct my_blk_dv *ptr_dev)
{
	/********Allocating Disk****************/
	ptr_dev->gd=alloc_disk(MY_BLK_MNR);
	if(ptr_dev->gd){
		printk(KERN_NOTICE"alloc_disk failed!!\n");
		return -ENOMEM;//ENOMEM=out of memory	
	}
	/***************************************/

	/*******Initializing struct gendisk*****/
	ptr_dev->gd->major=status;
	ptr_dev->gd->first_minor=0;
	ptr_dev->gd->fops=&my_blk_fops;
	ptr_dev->gd->queue=ptr_dev->que;
	ptr_dev->gd->private_data=ptr_dev;
	snprintf(ptr_dev->gd->disk_name,32,"abd");//?
	set_capacity(ptr_dev->gd,NR_SECTORS);//?
	/***************************************/
	
	/******************************************/
	/*To inform the kernel about the device sector size, 
	 *a parameter of the request queue must be set just 
	 *after the request queue is allocated, using the 
	 *blk_queue_logical_block_size() function*/

	blk_queue_logical_block_size(ptr_dev->que, 512);
        /******************************************/

	/*********Adding Disk to the system*****/

	/*Note that immediately after calling the add_disk() function (actually even during the call), 
	 * the disk is active and its methods can be called at any time. 
	 * As a result, this function SHOULD NOT BE CALLED before the driver is 
	 * fully initialized and ready to respond to requests for the registered disk.*/
	add_disk(ptr_dev->gd);
	/***************************************/
	return 0;
}

static int my_block_init(void)
{
	//#1================REGISTER BLOCK DRIVER====================
	
	status = register_blkdev(MY_BLOCK_MAJOR,My_BLKDEV_NAME);
	if(status < 0){
		printk(KERN_ERR"Block Device not registering\n");
	}

	//#2===============CREATE A BLOCK DEVICE====================
		
	create_block_device(&dev);//alloc_disk & add_disk
	
 return 0;	
}module_init(my_block_init);

static int del_blk_dv(struct my_blk_dv *ptr_dev)
{

	if(ptr_dev->gd)//check first if struct gendisk exist or not
		/*********Deallocating Disk*****************/
		del_gendisk(ptr_dev->gd);
	return 0;
}

static void my_block_exit(void)
{
	unregister_blkdev(status,My_BLKDEV_NAME);
	//===================DELETING THE BLOCK DEVICE====================
	
	/*Problem --> After a call to del_gendisk(), the 
	 *struct gendisk structure may continue to exist (and the 
	 *device operations may still be called) if there are still users
	 *Example: an open operation was called on the device but 
	 *the associated release operation has not been called*/
	
	/*One Solution --> keep the number of users of the device 
	 *and call the del_gendisk() function only when there are 
	 *no users left of the device*/

	//while(count--)
		//{/*operation of removing every user who called open operation} NB--> count can be the size of request queue
		del_blk_dv(&dev);// 
}module_exit(my_block_exit);

