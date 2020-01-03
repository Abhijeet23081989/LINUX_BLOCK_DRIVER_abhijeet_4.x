#include<linux/fs.h>
#include<linux/module.h>
#include<linux/init.h>
#define MY_BLOCK_MAJOR 240
#define My_BLKDEV_NAME "BLOCKADE"

static int my_block_init(void)
{
	int status;
	status = register_blkdev(MY_BLOCK_MAJOR,My_BLKDEV_NAME);
	if(status < 0){
		printk(KERN_ERR"Block Device not registering\n");
	}
	
 return 0;	
}module_init(my_block_init);

static void my_block_exit(void)
{
	unregister_blkdev(MY_BLOCK_MAJOR,My_BLKDEV_NAME);
}module_exit(my_block_exit);
