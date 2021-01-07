#include<linux/fs.h>//header for register_blkdev() & unregister_blkdev()
#include<linux/module.h>
#include<linux/init.h>
#include<linux/genhd.h>

#define MY_BLOCK_MAJOR 0 //default block number should be zero so that the kernal can allot the major number
#define MY_BLK_MNR 1
#define My_BLKDEV_NAME "BLOCKADE"
#define NR_SECTORS 1024//?
#define KERNEL_SECTOR_SIZE 512//?


//++++++++++++Block device structure to store important elements describing the device+++++++++
/*Struct gendisk has major and minor number both,in alloc_disk only minor number along with some other parameters are assigned but not major number*/
static struct my_blk_dv
{
	spinlock_t lock;//for mutual exclusion
	struct gendisk *gd;//struct gendisk is the basic structure in working with block devices
	struct request_queue *que;//?
}dev;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++Struct Block Device Operations++++++++++++++++++++++++++++++++++++++++++

struct block_device_operations{ 
	.owner = THIS_MODULE,
	.open  = my_blk_open,
	.release = my_blk_release,
}my_blk_fops;

//*****************Declaration*********************

static int create_block_device(struct my_blk_dv*);
static int del_blk_dv(struct my_blk_dv*);

//*************************************************

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int status;//status is to hold Major number and also the failed status of register_blkdev()
int count;//everytime open operation is called count goes +1 ,count goes -1 for every user who have called open operation and when count =0 del_gendisk() is called
//===============================================Block device open operation=========================================

static int my_block_open(struct block_device *bdev, fmode_t mode){
	printk(KERN_INFO"START BLOCK OPEN\n");
	printk(KERN_INFO"END BLOCK OPEN\n");
	return 0;
}

//===============================================Block device release/close operation=========================================
static int my_block_release(struct gendisk *gd, fmode_t mode){
	printk(KERN_INFO"START BLOCK RELEASE\n");
	printk(KERN_INFO"END BLOCK RELEASE\n");
	return 0;
}
/*No read or write operation as these operation will be performed by request functions*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static int my_block_init(void)
{
	//#1================REGISTER BLOCK DRIVER====================
	
	status = register_blkdev(MY_BLOCK_MAJOR,My_BLKDEV_NAME);
	if(status < 0){
		printk(KERN_ERR"Block Device not registering\n");
		return -EBUSY;
	}

	//#2===============CREATE A BLOCK DEVICE====================
		
	status=create_block_device(&dev);//alloc_disk & add_disk
	if(status<0)
		return status;

	//#3
	
 return 0;	
}module_init(my_block_init);

static int create_block_device(struct my_blk_dv *ptr_dev)
{
	/********Allocating Disk****************/
	ptr_dev->gd=alloc_disk(MY_BLK_MNR);
	if(!ptr_dev->gd){
		printk(KERN_NOTICE"alloc_disk failure\n");
		return -ENOMEM;
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

	/*********Adding Disk to the system*****/

	/*Note that immediately after calling the add_disk() function (actually even during the call), 
	 * the disk is active and its methods can be called at any time. 
	 * As a result, this function SHOULD NOT BE CALLED before the driver is 
	 * fully initialized and ready to respond to requests for the registered disk.*/
	add_disk(ptr_dev->gd);
	/***************************************/
	return status;
}

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
	//while(count--)
		//{operation of removing every user who called open operation}
		del_blk_dv(&dev);// 
	/*Problem --> After a call to del_gendisk(), the 
	 *struct gendisk structure may continue to exist (and the 
	 *device operations may still be called) if there are still users
	 *Example: an open operation was called on the device but 
	 *the associated release operation has not been called*/
	
	/*One Solution --> keep the number of users of the device 
	 *and call the del_gendisk() function only when there are 
	 *no users left of the device*/

}module_exit(my_block_exit);
