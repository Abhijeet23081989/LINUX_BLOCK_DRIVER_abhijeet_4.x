#include<linux/fs.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/genhd.h>
#define MY_BLOCK_MAJOR 0
#define MY_BLK_MNR 1
#define My_BLKDEV_NAME "BLOCKADE"

//++++++++++++Block device structure to store important elements describing the device+++++++++

static struct my_blk_dv
{
	struct gendisk *gd;//struct gendisk is the basic structure in working with block devices
}dev;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int status;

static int my_block_init(void)
{
	//#1================REGISTER BLOCK DRIVER====================
	
	status = register_blkdev(MY_BLOCK_MAJOR,My_BLKDEV_NAME);
	if(status < 0){
		printk(KERN_ERR"Block Device not registering\n");
	}

	//#2===============CREATE A BLOCK DEVICE====================
		
	create_block_device(&dev);

	//3#
	
 return 0;	
}module_init(my_block_init);

static int create_block_device(struct my_blk_dv *ptr_dev)
{
	/********Allocating Disk****************/
	ptr_dev->gd=alloc_disk(MY_BLK_MNR);
	/*********Adding Disk to the system*****/

	/*Note that immediately after calling the add_disk() function (actually even during the call), 
	 * the disk is active and its methods can be called at any time. 
	 * As a result, this function SHOULD NOT BE CALLED before the driver is 
	 * fully initialized and ready to respond to requests for the registered disk.*/
	add_disk(ptr_dev->gd);
}

static int del_blk_dv(struct my_block_dv *ptr_dev)
{

	if(ptr_dev->gd)//check first if struct gendisk exist or not
		/*********Deallocating Disk*****************/
		del_gendisk(ptr_dev->gd);
}
static void my_block_exit(void)
{
	unregister_blkdev(status,My_BLKDEV_NAME);
	//===================DELETING THE BLOCK DEVICE====================
		del_blk_dv(&dev);
}module_exit(my_block_exit);
