#include<linux/fs.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/genhd.h>
#define MY_BLOCK_MAJOR 240
#define My_BLKDEV_NAME "BLOCKADE"

//++++++++++++Block device structure to store important elements describing the device+++++++++

static struct my_blk_dv
{
	struct gendisk *gd;
}dev;

//+++++++++++++++++++++++++++++++++++++++++++

static int my_block_init(void)
{
	int status;
	//#1================REGISTER BLOCK DRIVER====================
	
	status = register_blkdev(MY_BLOCK_MAJOR,My_BLKDEV_NAME);
	if(status < 0){
		printk(KERN_ERR"Block Device not registering\n");
	}

	//#2===============CREATE A BLOCK DEVICE====================
		
	create_block_device();
	
 return 0;	
}module_init(my_block_init);

static int create_block_device()
{

}

static void my_block_exit(void)
{
	unregister_blkdev(MY_BLOCK_MAJOR,My_BLKDEV_NAME);
}module_exit(my_block_exit);
